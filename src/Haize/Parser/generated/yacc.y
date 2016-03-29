
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

namespace generated
{
	enum ParamPrefix
	{
		IN 	= 0x1,
		OUT	= 0x2,
	};
}

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

%token <node>	K_IF K_THEN K_ELSE K_FOR K_WHILE K_SWITCH
%token <node>	K_CONTINUE K_BREAK K_RETURN
%token <node>	K_GLOBAL K_NAMESPACE K_CLASS

// ********************
%type <node>	chunk

%type <node>	func_decl
				args_list_decl
				args_decl
				args_call

%type <integer>	arg_prefix

%type <node>	namespace_decl
				class_decl
				class_body

%type <node>	var_type
				variable variable_accessor
				func_call
				constant

%type <node>	assign_op
				binary_op
				expr
				lvalue

%type <node>	stmt_list
				stmt

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
	| program namespace_decl	{ m_node->addChild($2);}
	| program func_decl			{ m_node->addChild($2);}
	| program class_decl		{ m_node->addChild($2);}
	;

chunk
	: /* E */					{ $$ = AST_NODE_N(NT_CHUNK); }
	| chunk namespace_decl		{ $1->addChild($2); $$ = $1; }
	| chunk func_decl			{ $1->addChild($2); $$ = $1; }
	| chunk class_decl			{ $1->addChild($2); $$ = $1; }
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

variable
	: V_IDENTIFIER						{ $$ = AST_NODE(V_IDENTIFIER); EXTRACT_STR($1, $$->name); }
	| variable_accessor V_IDENTIFIER	
		{ 
			hz::parser::ASTNode* nIdent = AST_NODE(V_IDENTIFIER); 
			EXTRACT_STR($2, nIdent->name); 
			$1->addChild(nIdent);
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
	: V_IDENTIFIER S_ACCESSOR
		{
			$$ = AST_NODE_N(S_ACCESSOR);
			hz::parser::ASTNode* nIdent = AST_NODE(V_IDENTIFIER); 
			EXTRACT_STR($1, nIdent->name); 
			$$->addChild(nIdent);
		}
	| variable_accessor V_IDENTIFIER S_ACCESSOR
		{
			hz::parser::ASTNode* nAccess = AST_NODE_N(S_ACCESSOR); 
			hz::parser::ASTNode* nIdent = AST_NODE(V_IDENTIFIER); 
			EXTRACT_STR($2, nIdent->name); 
			$1->addChild(nAccess);
			nAccess->addChild(nIdent);
			$$ = nAccess;
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

var_global
	: K_GLOBAL var_type S_SEPARATOR
		{
		}
	;

namespace_decl
	: K_NAMESPACE V_IDENTIFIER S_LBRACE chunk S_RBRACE
		{
			m::String name = (*$2);
			MUON_DELETE($2);
			$$ = AST_NODE(NT_NAMESPACE, name);
			// Extract all child of chunk (as there is a NT_CHUNK token)
			for(m::u32 i = 0; i < $4->children->size(); ++i)
			{
				$$->addChild($4->children->at(i));
			}
			MUON_DELETE($4);
		}
	;

func_decl
	: var_type S_LPARENT args_list_decl S_RPARENT S_LBRACE stmt_list S_RBRACE
		{
			$$ = AST_NODE_N(NT_FUNCTION);
			$$->addChild($1);
			$$->addChild($3);
			$$->addChild($6);
		}
	;
args_list_decl
	: /* E */				{ $$ = AST_NODE_N(NT_ARGUMENTS); }
	| args_decl				{ $$ = $1; /* Node have been created by args_decl */}
	;
args_decl
	: arg_prefix var_type
		{
			generated::ParamPrefix prefix = (generated::ParamPrefix)$1;
			$$ = AST_NODE_N(NT_ARGUMENTS);
			$$->addChild($2);
		}
	| args_decl S_COMMA arg_prefix var_type
		{
			generated::ParamPrefix prefix = (generated::ParamPrefix)$3;
			$1->addChild($4);
		}
	;
arg_prefix
	: /* E */	{ $$ = generated::ParamPrefix::IN; }
	| K_IN		{ $$ = generated::ParamPrefix::IN; }
	| K_OUT		{ $$ = generated::ParamPrefix::OUT; }
	| K_INOUT	{ $$ = (generated::ParamPrefix::IN | generated::ParamPrefix::OUT); }
	;

class_decl
	: K_CLASS V_IDENTIFIER S_LBRACE class_body S_RBRACE
		{
			m::String name = (*$2);
			MUON_DELETE($2);
			$$ = AST_NODE(NT_CLASS, name);
		}
	;

class_body
	: /* E */		{ printf("class::E\n");}
	| var_type		{ printf("class::var_type\n"); }
	| func_decl		{ printf("class::func_decl\n"); $$ = $1; }
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
	: /* E */				{ $$ = AST_NODE_N(NT_ARGUMENTS); }
	| lvalue S_COMMA		{ $$ = AST_NODE_N(NT_ARGUMENTS); $$->addChild($1); }
	| args_call lvalue		{ $$ = $1; $$->addChild($2); }
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

stmt_list
	: /* E */						{ $$ = AST_NODE_N(NT_STMT); }
	| stmt S_SEPARATOR				{ $$ = AST_NODE_N(NT_STMT); $$->addChild($1); }
	| stmt_list stmt S_SEPARATOR 	{ $$ = $1; $$->addChild($2); }
	;

assign_op
	: MATH_ASN	{ $$ = AST_NODE_N(MATH_ASN); }
	;

stmt
	: lvalue						{ $$ = $1; }
	| variable assign_op expr		{ $$ = $2; $$->addChild($1); $$->addChild($3); }
//	| loop_control					{ $$; }
//	| cond_control					{ $$; }
	;

binary_op
	: MATH_ADD	{ $$ = AST_NODE_N(MATH_ADD); }
	| MATH_SUB	{ $$ = AST_NODE_N(MATH_SUB); }
	| MATH_MUL	{ $$ = AST_NODE_N(MATH_MUL); }
	| MATH_DIV	{ $$ = AST_NODE_N(MATH_DIV); }
	| MATH_MOD	{ $$ = AST_NODE_N(MATH_MOD); }
	;


expr
	: MATH_SUB expr					{ $$ = AST_NODE_N(UNARY_MINUS); $$->addChild($2); }	%prec UNARY_MINUS
	| MATH_ADD expr					{ $$ = $2; }										%prec UNARY_PLUS 
	| lvalue						{ $$ = $1; }
	| S_LPARENT expr S_RPARENT		{ $$ = $2; }
	| expr binary_op expr
		{
			$$ = $2;
			$$->addChild($1);
			$$->addChild($3);
		}
	;

lvalue
	: variable		{ $$ = $1; }
	| constant		{ $$ = $1; }
	| func_call		{ $$ = $1; }
	;

