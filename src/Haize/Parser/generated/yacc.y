
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
#include "Haize/Parser/Token.hpp"
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

#include "flex.l.hpp"

extern void yyerror(YYLTYPE*, yyscan_t, struct hz::parser::ASTNode*, struct hz::Error*, const char*);
%}


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

// ********************
// Non-Terminal or Syntaxic-only Token
// NT_ROOT is used inside cpp code
// ********************

%token <node>	NT_ROOT
				NT_FUNCTION
				NT_FUNCTION_ARGUMENTS
				NT_FUNCTION_BODY

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
%type <integer>	arg_prefix

%type <node>	namespace_decl
				class_decl

%type <node>	var_type

// **********************************************
// Rules
// **********************************************
%start chunk
%%

chunk
	: /* E */					{ $$ = m_node; }
	| chunk namespace_decl		{ $$->addChild($2); }
	| chunk func_decl			{ $$->addChild($2); }
	| chunk class_decl			{ $$->addChild($2); }
	;

//asnop
//	: MATH_ASN
//	;

//binop
//	: MATH_ADD
//	| MATH_SUB
//	| MATH_MUL
//	| MATH_DIV
//	| MATH_MOD
//	;

//unary
//	: UNARY_MINUS expr
//	;

//constant
//	: V_NIL
//	| V_TRUE
//	| V_FALSE
//	| V_STRING
//	| V_NUMBER
//	;

//variable
//	: V_IDENTIFIER
//	;

var_type
	: V_IDENTIFIER V_IDENTIFIER
		{
			m::String type = (*$1);
			m::String var = (*$2);
			MUON_DELETE($1);
			MUON_DELETE($2);
			// Node name is the variable name
			// Token value is the return type
			hz::parser::ASTNode* nodeType = MUON_NEW(hz::parser::ASTNode, V_IDENTIFIER, var);
			nodeType->token.value = type;

			$$ = nodeType;
			$$->token.column = yyloc.first_line;
			$$->token.column = yyloc.first_column;
		}
	;

//var_global
//	: K_GLOBAL var_type
//	;

namespace_decl
	: K_NAMESPACE V_IDENTIFIER S_LBRACE chunk S_RBRACE
	;

func_decl
	: var_type S_LPARENT args_list_decl S_RPARENT S_LBRACE S_RBRACE
		{
			$$ = MUON_NEW(hz::parser::ASTNode, NT_FUNCTION, "#FUNCTION#");
			$$->addChild($1);
			$$->addChild($3);
		}
	;
args_list_decl
	: /* E */				{ $$ = MUON_NEW(hz::parser::ASTNode, NT_FUNCTION_ARGUMENTS, "#ARGS#"); }
	| args_decl				{ $$ = $1; /* Node have been created by args_decl */}
	;
args_decl
	: arg_prefix var_type
		{
			generated::ParamPrefix prefix = (generated::ParamPrefix)$1;
			$$ = MUON_NEW(hz::parser::ASTNode, NT_FUNCTION_ARGUMENTS, "#ARGS#");
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
	: K_CLASS
	;

//func_call
//	: V_IDENTIFIER S_LPARENT args_call S_RPARENT S_LBRACE func_body S_RBRACE
//	;
//args_call
//	: /* E */
//	| expr S_COMMA
//	| args_call expr
//	;

//func_body
//	: /* E */
//	| expr
//	;

// TODO: COMPLET
//cond_control
//	: K_IF
//	| K_ELSE
//	;

// TODO: COMPLET
//loop_control
//	: K_FOR
//	| K_WHILE
//	| K_SWITCH
//	;

//flow_control
//	: K_BREAK S_SEPARATOR
//	| K_CONTINUE S_SEPARATOR
//	| K_RETURN expr
//	;

// TODO: COMPLET
//expr
//	: S_LPARENT expr S_RPARENT
//	| variable
//	| constant
//	| func_call
//	| expr binop constant S_SEPARATOR
//	| expr binop variable S_SEPARATOR
//	;

// TODO
//array
//	: S_LBRACKET S_RBRACKET
//	;
