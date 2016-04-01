
// **********************************************
// Configuration
// **********************************************
%pure-parser
%locations

%lex-param		{void* scanner}
%parse-param	{void* scanner}
%parse-param	{hz::parser::ASTNode* m_node}
%parse-param	{hz::Error* m_error}

%union {
	m::String* string;
	int integer;
	float floating;
	bool boolean;
	hz::parser::ASTNode* node;
}

// **********************************************
// Include & Declaration
// **********************************************

%code requires {
#include <Muon/String.hpp>
#include "Haize/Error.hpp"
#include "Haize/Parser/ASTNode.hpp"

}


%{
#pragma warning(disable: 4065) // switch statement contains 'default' but no 'case' labels

#include <Muon/Core/Define.hpp>
#include "flex.l.hpp"

extern void yyerror(YYLTYPE*, yyscan_t, struct hz::parser::ASTNode*, struct hz::Error*, const char*);
#if defined(MUON_PLATFORM_WINDOWS)
#	define AST_NODE(...)		MUON_NEW(hz::parser::ASTNode, __VA_ARGS__)
#else
#	define AST_NODE(args...)	MUON_NEW(hz::parser::ASTNode, ##args)
#endif
#define AST_NODE_N(id)	MUON_NEW(hz::parser::ASTNode, id, MUON_STR(id))
#define EXTRACT_STR(str, out) do {out = (m::String)*str; MUON_DELETE(str); } while(false);
%}

// ********************
// Non-Terminal or Syntaxic-only Token
// NT_ROOT is used inside cpp code
// ********************

%token <node>	NT_ROOT	NT_CHUNK
				NT_NAMESPACE
				NT_FUNCTION
				NT_ARGUMENTS
				NT_CLASS
				NT_STMT

%token <node>	S_LPARENT S_RPARENT S_LBRACE S_RBRACE S_LBRACKET S_RBRACKET
%token <node>	S_COMMA S_SEPARATOR S_NEWLINE

%token <string>		V_IDENTIFIER
%token <string>		V_STRING
%token <floating>	V_FLOATING V_NUMBER
%token <integer>	V_TRUE V_FALSE V_NIL V_INTEGER
%token <integer>	K_IN K_OUT K_INOUT

%token <node>	K_NEW K_DELETE
%token <node>	K_IF K_THEN K_ELSE K_FOR K_WHILE K_SWITCH
%token <node>	K_CONTINUE K_BREAK K_RETURN
%token <node>	K_GLOBAL K_NAMESPACE K_CLASS

// ********************
%type <node>	chunk

%type <node>	func_decl args_decl
				func_call args_call

%type <integer>	arg_prefix

%type <node>	namespace_decl
				class_decl
				class_body
				global_decl

%type <node>	var_type
				variable variable_accessor var_or_func
				constant

%type <node>	assign_op
				binary_op
				expr

%type <node>	stmt_list
				stmt
				new_variable
				delete_variable

// **********************************************
// Token List
// **********************************************

%right	<node>	MATH_ASN
		MATH_ASN_ADD MATH_ASN_SUB MATH_ASN_MUL MATH_ASN_DIV MATH_ASN_MOD
		MATH_ASN_BITWISE_OR MATH_ASN_BITWISE_AND
		MATH_ASN_BITWISE_XOR MATH_ASN_BITWISE_NOT
		MATH_ASN_BITWISE_LSH MATH_ASN_BITWISE_RSH
%left	<node>	LOGIC_OR
%left	<node>	LOGIC_AND
%left	<node>	BITWISE_OR
%left	<node>	BITWISE_XOR
%left	<node>	BITWISE_AND
%left	<node>	LOGIC_EQ LOGIC_NEQ
%left	<node>	LOGIC_LT LOGIC_GT LOGIC_LET LOGIC_GET
%left	<node>	BITWISE_LSH BITWISE_RSH
%left	<node>	MATH_ADD MATH_SUB
%left	<node>	MATH_MUL MATH_DIV MATH_MOD
%right	<node>	MATH_PREFINC MATH_PREFDEC LOGIC_NOT BITWISE_NOT UNARY_MINUS UNARY_PLUS
%left	<node>	MATH_POSTINC MATH_POSTDEC
%left	<node>	S_ACCESSOR

// **********************************************
// Rules
// **********************************************
%start program
%%
program
	: /* E */					{ }
	| program namespace_decl	{ m_node->addChild($2); }
	| program func_decl			{ m_node->addChild($2); }
	| program class_decl		{ m_node->addChild($2); }
	| program global_decl		{ m_node->addChild($2); }
	;

chunk
	: /* E */					{ $$ = AST_NODE_N(NT_CHUNK); }
	| chunk namespace_decl		{ $1->addChild($2); $$ = $1; }
	| chunk func_decl			{ $1->addChild($2); $$ = $1; }
	| chunk class_decl			{ $1->addChild($2); $$ = $1; }
	| chunk global_decl			{ $1->addChild($2); $$ = $1; }
	;

constant
	: V_NIL			{ $$ = AST_NODE_N(V_NIL); }
	| V_TRUE		{ $$ = AST_NODE_N(V_TRUE); }
	| V_FALSE		{ $$ = AST_NODE_N(V_FALSE); }
	| V_STRING
		{
			$$ = AST_NODE_N(V_STRING);
			EXTRACT_STR($1, $$->value);
		}
	| V_NUMBER
		{
			$$ = AST_NODE_N(V_NUMBER);
			$$->value = $1;
		}
	| V_INTEGER
		{
			$$ = AST_NODE_N(V_INTEGER);
			$$->value = $1;
		}
	;

var_or_func
	: V_IDENTIFIER		{ $$ = AST_NODE(V_IDENTIFIER); EXTRACT_STR($1, $$->name); }
	| func_call			{ $$ = $1; }
	;

variable
	: var_or_func						{ $$ = $1; }
	| variable_accessor var_or_func
		{ 
			$1->addChild($2);
			// Variable is the highest "accessor" level
			hz::parser::ASTNode* accessor = $1;
			while(accessor->parent && accessor->parent->type == S_ACCESSOR)
			{
				accessor = accessor->parent;
			}
			$$ = accessor;
		}
	;

variable_accessor
	: var_or_func S_ACCESSOR
		{
			$$ = AST_NODE_N(S_ACCESSOR);
			$$->addChild($1);
		}
	| variable_accessor var_or_func S_ACCESSOR
		{
			$$ = AST_NODE_N(S_ACCESSOR);
			$1->addChild($$);
			$$->addChild($2);
		}
	;

var_type
	: V_IDENTIFIER V_IDENTIFIER
		{
			m::String var = (*$2);
			MUON_DELETE($2);
			// Node name is the variable name
			// Token value is the return type
			hz::parser::ASTNode* nodeType = AST_NODE(V_IDENTIFIER, var);
			EXTRACT_STR($1, nodeType->value);

			$$ = nodeType;
		}
	;

namespace_decl
	: K_NAMESPACE V_IDENTIFIER S_LBRACE chunk S_RBRACE
		{
			$$ = AST_NODE(NT_NAMESPACE);
			EXTRACT_STR($2, $$->name);
			// Extract all child of chunk (as there is a NT_CHUNK token)
			for(m::u32 i = 0; i < $4->children->size(); ++i)
			{
				$$->addChild($4->children->at(i));
			}
			MUON_DELETE($4);
		}
	;

func_decl
	: var_type S_LPARENT args_decl S_RPARENT S_LBRACE stmt_list S_RBRACE
		{
			$$ = AST_NODE_N(NT_FUNCTION);
			$$->addChild($1);
			$$->addChild($3);
			$$->addChild($6);
		}
	;
args_decl
	: /* E */									{ $$ = AST_NODE_N(NT_ARGUMENTS); }
	| arg_prefix var_type						{ $$ = AST_NODE_N(NT_ARGUMENTS); $$->addChild($2); $$->value = $1; }
	| args_decl S_COMMA arg_prefix var_type		{ $1->addChild($4); $4->value = $3; }
	;

arg_prefix
	: /* E */	{ $$ = (m::u32)hz::parser::ParamPrefix::IN; }
	| K_IN		{ $$ = (m::u32)hz::parser::ParamPrefix::IN; }
	| K_OUT		{ $$ = (m::u32)hz::parser::ParamPrefix::OUT; }
	| K_INOUT	{ $$ = (m::u32)(hz::parser::ParamPrefix::IN | hz::parser::ParamPrefix::OUT); }
	;

class_decl
	: K_CLASS V_IDENTIFIER S_LBRACE class_body S_RBRACE
		{
			$$ = AST_NODE(NT_CLASS);
			EXTRACT_STR($2, $$->name);
		}
	;

class_body
	: /* E */					{ printf("class::E\n");}
	| class_body var_type		{ printf("class::var_type\n"); }
	| class_body func_decl		{ printf("class::func_decl\n"); $$ = $1; }
	;

func_call
	: V_IDENTIFIER S_LPARENT args_call S_RPARENT
		{
			$$ = AST_NODE_N(NT_FUNCTION);
			EXTRACT_STR($1, $$->name);
			$$->addChild($3);
		}
	;
args_call
	: /* E */						{ $$ = AST_NODE_N(NT_ARGUMENTS); }
	| expr 							{ $$ = AST_NODE_N(NT_ARGUMENTS); $$->addChild($1); }
	| args_call S_COMMA expr		{ $$ = $1; $$->addChild($3); }
	;

/*
cond_control
	: K_IF
	| K_ELSE
	;

loop_control
	: K_FOR
	| K_WHILE
	| K_SWITCH
	;

flow_control
	: K_BREAK S_SEPARATOR
	| K_CONTINUE S_SEPARATOR
	| K_RETURN expr
	;

array
	: S_LBRACKET S_RBRACKET
	;
// */

global_decl
	: K_GLOBAL var_type	S_SEPARATOR		{ $$ = AST_NODE_N(K_GLOBAL); $$->addChild($2); }
	;

stmt_list
	: /* E */						{ $$ = AST_NODE_N(NT_STMT); printf("stmt::E: %p\n", $$); }
	| stmt S_SEPARATOR stmt_list 	{ $$ = $3; $$->addChild($1); }
	;

assign_op
	: MATH_ASN					{ $$ = AST_NODE_N(MATH_ASN); }
	| MATH_ASN_ADD				{ $$ = AST_NODE_N(MATH_ASN_ADD); }
	| MATH_ASN_SUB				{ $$ = AST_NODE_N(MATH_ASN_SUB); }
	| MATH_ASN_MUL				{ $$ = AST_NODE_N(MATH_ASN_MUL); }
	| MATH_ASN_DIV				{ $$ = AST_NODE_N(MATH_ASN_DIV); }
	| MATH_ASN_MOD				{ $$ = AST_NODE_N(MATH_ASN_MOD); }
	| MATH_ASN_BITWISE_OR		{ $$ = AST_NODE_N(MATH_ASN_BITWISE_OR); }
	| MATH_ASN_BITWISE_AND		{ $$ = AST_NODE_N(MATH_ASN_BITWISE_AND); }
	| MATH_ASN_BITWISE_XOR		{ $$ = AST_NODE_N(MATH_ASN_BITWISE_XOR); }
	| MATH_ASN_BITWISE_NOT		{ $$ = AST_NODE_N(MATH_ASN_BITWISE_NOT); }
	| MATH_ASN_BITWISE_LSH		{ $$ = AST_NODE_N(MATH_ASN_BITWISE_LSH); }
	| MATH_ASN_BITWISE_RSH		{ $$ = AST_NODE_N(MATH_ASN_BITWISE_RSH); }
	;

new_variable
	: K_NEW var_type							{ $$ = AST_NODE_N(K_NEW); $$->addChild($2); }
	| K_NEW var_type MATH_ASN expr				{ $$ = AST_NODE_N(K_NEW); $$->addChild($2); $$->addChild($4); }
	| K_NEW var_type S_LPARENT expr	S_RPARENT	{ $$ = AST_NODE_N(K_NEW); $$->addChild($2); $$->addChild($4); }
	;

delete_variable
	: K_DELETE S_LPARENT V_IDENTIFIER S_RPARENT		{ $$ = AST_NODE_N(K_DELETE); EXTRACT_STR($3, $$->value); }
	| K_DELETE V_IDENTIFIER							{ $$ = AST_NODE_N(K_DELETE); EXTRACT_STR($2, $$->value); }
	;

stmt
	: constant						{ $$ = $1; }
	| variable						{ $$ = $1; }
	| variable assign_op expr		{ $$ = $2; $$->addChild($1); $$->addChild($3); }
	| new_variable					{ $$ = $1; }
	| delete_variable				{ $$ = $1; }
	| K_RETURN expr					{ $$ = AST_NODE_N(K_RETURN); $$->addChild($2); }
//	| loop_control					{ $$ = $1; }
//	| cond_control					{ $$ = $1; }
	;

binary_op
	: MATH_ADD	{ $$ = AST_NODE_N(MATH_ADD); }
	| MATH_SUB	{ $$ = AST_NODE_N(MATH_SUB); }
	| MATH_MUL	{ $$ = AST_NODE_N(MATH_MUL); }
	| MATH_DIV	{ $$ = AST_NODE_N(MATH_DIV); }
	| MATH_MOD	{ $$ = AST_NODE_N(MATH_MOD); }

	| LOGIC_EQ	{ $$ = AST_NODE_N(LOGIC_EQ); }
	| LOGIC_NEQ	{ $$ = AST_NODE_N(LOGIC_NEQ); }
	| LOGIC_LT	{ $$ = AST_NODE_N(LOGIC_LT); }
	| LOGIC_LET	{ $$ = AST_NODE_N(LOGIC_LET); }
	| LOGIC_GT	{ $$ = AST_NODE_N(LOGIC_GT); }
	| LOGIC_GET	{ $$ = AST_NODE_N(LOGIC_GET); }
	| LOGIC_NOT	{ $$ = AST_NODE_N(LOGIC_NOT); }

	| BITWISE_OR	{ $$ = AST_NODE_N(BITWISE_OR); }
	| BITWISE_XOR	{ $$ = AST_NODE_N(BITWISE_XOR); }
	| BITWISE_AND	{ $$ = AST_NODE_N(BITWISE_AND); }
	| BITWISE_LSH	{ $$ = AST_NODE_N(BITWISE_LSH); }
	| BITWISE_RSH	{ $$ = AST_NODE_N(BITWISE_RSH); }
	| BITWISE_NOT	{ $$ = AST_NODE_N(BITWISE_NOT); }

	;


expr
	: MATH_SUB expr					{ $$ = AST_NODE_N(UNARY_MINUS); $$->addChild($2); }	%prec UNARY_MINUS
	| MATH_ADD expr					{ $$ = $2; }										%prec UNARY_PLUS 
	| MATH_PREFINC expr				{ $$ = AST_NODE_N(MATH_PREFINC); $$->addChild($2); }
	| MATH_PREFDEC expr				{ $$ = AST_NODE_N(MATH_PREFDEC); $$->addChild($2); }
	| variable		{ $$ = $1; }
	| constant		{ $$ = $1; }
	| S_LPARENT expr S_RPARENT		{ $$ = $2; }
	| expr binary_op expr
		{
			$$ = $2;
			$$->addChild($1);
			$$->addChild($3);
		}
	;
