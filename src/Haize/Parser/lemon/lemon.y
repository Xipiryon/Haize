
// **********************************************
// Configuration
// **********************************************

%token_type { const char* }
%extra_argument { ExtraParserContext* epc }

%syntax_error {
	epc->noError = false;
	epc->log(m::LOG_ERROR) << "Failure while constructing the AST" << m::endl;
}

%include {
	#include <iostream>
	#include <cassert>
	#include "Haize/Parser/Token.hpp"
	#include "ExtraParserContext.hpp"
}

%start_symbol  start

// **********************************************
// Token List
// **********************************************

%right	MATH_ASN
		MATH_ASN_ADD MATH_ASN_SUB MATH_ASN_MUL MATH_ASN_DIV MATH_ASN_MOD
		MATH_ASN_BITWISE_OR MATH_ASN_BITWISE_AND
		MATH_ASN_BITWISE_XOR MATH_ASN_BITWISE_NOT
		MATH_ASN_BITWISE_LSH MATH_ASN_BITWISE_RSH.
%left	LOGIC_OR.
%left	LOGIC_AND.
%left	BITWISE_OR.
%left	BITWISE_XOR.
%left	BITWISE_AND.
%left	LOGIC_EQ LOGIC_NEQ.
%left	LOGIC_LT LOGIC_GT LOGIC_LET LOGIC_GET.
%left	BITWISE_LSH BITWISE_RSH.
%left	MATH_ADD MATH_SUB.
%left	MATH_MUL MATH_DIV MATH_MOD.
%right	MATH_PREFINC MATH_PREFDEC LOGIC_NOT BITWISE_NOT UNARY_PLUS UNARY_MINUS.
%left	MATH_POSTINC MATH_POSTDEC.
%left	S_ACCESSOR.

// **********************************************
// Rules
// **********************************************
start ::= chunk.	{}
start ::=.			{} // Allow empty files (or only commented files)

chunk ::= chunk namespace_decl.
chunk ::= chunk func_decl.
chunk ::= chunk class_decl.

asnop ::= MATH_ASN.
binop ::= MATH_ADD.
binop ::= MATH_SUB.
binop ::= MATH_MUL.
binop ::= MATH_DIV.
binop ::= MATH_MOD.

unary ::= UNARY_MINUS expr.

const ::= V_NIL.
const ::= V_TRUE.
const ::= V_FALSE.
const ::= V_STRING.
const ::= V_NUMBER.

variable ::= V_IDENTIFIER.

var_type ::= V_IDENTIFIER V_IDENTIFIER. { printf("var_type\n"); }
var_global ::= K_GLOBAL var_type.

namespace_decl ::= K_NAMESPACE V_IDENTIFIER S_LBRACE chunk S_RBRACE.

func_decl ::= var_type S_LPARENT m_args_decl S_RPARENT.
m_args_decl ::=.
m_args_decl ::= args_decl.
args_decl ::= arg_decl S_COMMA.
args_decl ::= args_decl arg_decl.
arg_decl ::= arg_prefix var_type.
arg_prefix ::=.
arg_prefix ::= K_IN.
arg_prefix ::= K_OUT.

class_decl ::= K_CLASS.

func_call ::= V_IDENTIFIER S_LPARENT args_call S_RPARENT S_LBRACE func_body S_RBRACE.
args_call ::=.
args_call ::= expr S_COMMA.
args_call ::= args_call expr.
func_body ::=.
func_body ::= expr.

// TODO: COMPLETE
cond_control ::= K_IF.
cond_control ::= K_ELSE.

// TODO: COMPLETE
loop_control ::= K_FOR.
loop_control ::= K_WHILE.
loop_control ::= K_SWITCH.

flow_control ::= K_BREAK S_SEPARATOR.
flow_control ::= K_CONTINUE S_SEPARATOR.
flow_control ::= K_RETURN expr.

// TODO: COMPLETE
expr ::= S_LPARENT expr S_RPARENT.
expr ::= variable.
expr ::= constant.
expr ::= func_call.
expr ::= expr binop constant S_SEPARATOR.
expr ::= expr binop variable S_SEPARATOR.

// TODO:
array ::= S_LBRACKET S_RBRACKET.
