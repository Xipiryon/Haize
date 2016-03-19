
// **********************************************
// Configuration
// **********************************************
%define api.pure full

%union  {
}
// **********************************************
// Include & Declaration
// **********************************************
%{
#pragma warning(disable: 4065) // switch statement contains 'default' but no 'case' labels

#include <Muon/String.hpp>
#include "flex.l.hpp"

extern void yyerror(const char*);
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
%right	<node>	MATH_PREFINC MATH_PREFDEC LOGIC_NOT BITWISE_NOT UNARY_SIGN
%left	<node>	MATH_POSTINC MATH_POSTDEC OP_ACCESSOR
%left	<node>	S_ACCESSOR

// ********************
// Non-Terminal or Syntaxic-only Token
// NT_ROOT is used inside cpp code
// ********************

%token <node>	NT_ROOT NT_EMPTY
%token <node>	NT_STRUCT_BODY NT_ATTR_DECL
%token <node>	NT_FUNC_CALL NT_ARG_LIST
%token <node>	NT_STMT

%token			S_LPARENT S_RPARENT S_LBRACE S_RBRACE S_LBRACKET S_RBRACKET
%token <node>	S_COMMA S_SEPARATOR S_NEWLINE

%token <string>		V_IDENTIFIER
%token <string>		V_STRING
%token <floating>	V_FLOATING V_NUMBER
%token <integer>	V_TRUE V_FALSE V_NIL V_INTEGER

%token <node>	K_IF K_THEN K_ELSE K_FOR K_WHILE K_SWITCH
%token <node>	K_CONTINUE K_BREAK K_RETURN
%token <node>	K_IN K_OUT
%token <node>	K_GLOBAL K_NAMESPACE K_CLASS 

// ********************
/*
%type <node>	block block_empty chunk chunk_empty stmt_decl expr subexpr
%type <node>	namespace_decl
%type <node>	param_list param_decl func_decl func_call arg_list
%type <node>	attr_decl struct_body struct_body_decl struct_decl
%type <node>	expr_asn_op binop_all expr_bin_op expr_cmp_op expr_bit_op
%type <node>	conditional_block
%type <node>	constant variable variable_lval
*/

// **********************************************
// Rules
// **********************************************
%start program
%%

program
	:
	;