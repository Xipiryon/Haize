
%token_type {int}

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
%right	MATH_PREFINC MATH_PREFDEC LOGIC_NOT BITWISE_NOT UNARY_SIGN.
%left	MATH_POSTINC MATH_POSTDEC OP_ACCESSOR.
%left	OP_NAMESPACE.

%include {
	#include <iostream>
}

%syntax_error {
	std::cout << "Syntax error!" << std::endl;
}

chunk ::= .


asnop ::= MATH_ASN.
binop ::= MATH_ADD.
binop ::= MATH_SUB.
binop ::= MATH_MUL.
binop ::= MATH_DIV.
binop ::= MATH_MOD.

const ::= NIL.
const ::= TRUE.
const ::= FALSE.
const ::= STRING.
const ::= NUMBER.

var_type ::= IDENT IDENT.

func_call ::= ident S_LPARENT args_call S_RPARENT.
args_call ::= .
args_call ::= expr args_call_rec.
args_call_rec ::= S_COMMA args_call.

