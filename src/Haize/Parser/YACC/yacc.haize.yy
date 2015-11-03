%{
#pragma warning(disable: 4065) // switch statement contains 'default' but no 'case' labels

// Generated file will be created in a subfolder called "generated/"
// As we require some files that are not in "include/" folder,
// we must look for them in with a relative path way.
#include "../Extern.hpp"

// Looking for Haize Header, so it's ok
#include <Muon/String.hpp>
#include "Haize/Parser/ASTNode.hpp"
#include "Haize/Parser/Info.hpp"

#define HZ_NEW(T)	MUON_CNEW(hz::parser::ASTNode, T, #T)
%}

%union {
	muon::String* string;
	float floating;
	int integer;
	hz::parser::ASTNode* node;
}

/*
* Operator and Token Precedence
*/
%right	<node> MATH_ASN
		MATH_ASN_ADD MATH_ASN_SUB MATH_ASN_MUL MATH_ASN_DIV MATH_ASN_MOD
		MATH_ASN_BITWISE_OR MATH_ASN_BITWISE_AND
		MATH_ASN_BITWISE_XOR MATH_ASN_BITWISE_NOT
		MATH_ASN_BITWISE_LSH MATH_ASN_BITWISE_RSH
%left	<node> LOGIC_OR
%left	<node> LOGIC_AND
%left	<node> BITWISE_OR
%left	<node> BITWISE_XOR
%left	<node> BITWISE_AND
%left	<node> LOGIC_EQ LOGIC_NEQ
%left	<node> LOGIC_LT LOGIC_GT LOGIC_LET LOGIC_GET
%left	<node> BITWISE_LSH BITWISE_RSH
%left	<node> MATH_ADD MATH_SUB
%left	<node> MATH_MUL MATH_DIV MATH_MOD
%right	<node> MATH_PREFINC MATH_PREFDEC LOGIC_NOT BITWISE_NOT UNARY_SIGN
%left	<node> MATH_POSTINC MATH_POSTDEC OP_ACCESSOR
%left	<node> OP_NAMESPACE

/*
* Non-Terminal or Syntaxic-only Token
*/
%token <node> NT_ROOT NT_BLOCK NT_CHUNK NT_EMPTY NT_EOF
%token <node> NT_CLASS_DECL NT_CLASS_BODY NT_ATTR_DECL
%token <node> NT_FUNC_CALL NT_FUNC_DECL NT_ARG_LIST
%token <node> NT_STMT

%token S_LPARENT S_RPARENT S_LBRACE S_RBRACE S_LBRACKET S_RBRACKET
%token <node> S_COMMA S_SEPARATOR S_NEWLINE

%token <string> V_IDENT
%token <string> V_STRING
%token <floating> V_NUMBER
%token <integer> V_TRUE V_FALSE V_NIL

%token <node> IF THEN ELSE ELSEIF END FOR WHILE DO IN
%token <node> CLASS ATTR FUNCTION RETURN
%token <node> NAMESPACE

/*
* Declares our customs non terminal tokens
*/
%type <node> block block_empty chunk chunk_empty stmt_decl expr subexpr
%type <node> namespace_decl
%type <node> param_list param_decl func_decl func_call arg_list
%type <node> attr_decl class_body class_body_decl class_decl
%type <node> expr_asn_op binop_all expr_bin_op expr_cmp_op expr_bit_op
%type <node> conditional_block
%type <node> constant variable variable_lval

/*
* Let's start!
*/
%start program
%%

program
	: chunk		{ g_parseInfo->ASTRoot->addChild($1); }
	;

chunk
	: block				{ $$ = HZ_NEW(NT_BLOCK); if ($1 != NULL) { $$->addChild($1); } }
	| chunk block		{ $$ = $1; if($2 != NULL) { $$->addChild($2); } }
	;

chunk_empty
	: block_empty			{ $$ = HZ_NEW(NT_BLOCK); if ($1 != NULL) { $$->addChild($1); } }
	| chunk block			{ $$ = $1; if($2 != NULL) { $$->addChild($2); } }
	;

block
	: stmt_decl			{ if($1 == NULL) { $$ = NULL; } else { $$ = HZ_NEW(NT_STMT); $$->addChild($1); } }
	| func_decl			{ $$ = $1; }
	| class_decl		{ $$ = $1; }
	| namespace_decl	{ $$ = $1; }
	| conditional_block 		{ $$ = $1; }
	| RETURN expr S_SEPARATOR	{ $$ = HZ_NEW(RETURN); $$->addChild($2); }
	;

block_empty
	: /* E */			{ $$ = HZ_NEW(NT_EMPTY); }
	| block				{ $$ = $1; }
	;

namespace_decl
	: NAMESPACE V_IDENT S_LBRACE chunk_empty S_RBRACE	{ $$ = HZ_NEW(NAMESPACE); $$->value = *$2; $$->addChild($4); }
	;

stmt_decl
	: expr S_SEPARATOR							{ $$ = $1; }
	| variable expr_asn_op expr S_SEPARATOR		{ $$ = $2; $$->addChild($1); $$->addChild($3); }
	;

expr
	: subexpr						{ $$ = $1; }
	| expr binop_all subexpr		{ $$ = $2; $$->addChild($1); $$->addChild($3); }
	;

subexpr
	: constant								{ $$ = $1; }
	| variable								{ $$ = $1; }
	| MATH_ADD expr %prec UNARY_SIGN		{ $$ = HZ_NEW(UNARY_SIGN); $$->value = true; $$->addChild($2); }
	| MATH_SUB expr %prec UNARY_SIGN		{ $$ = HZ_NEW(UNARY_SIGN); $$->value = false; $$->addChild($2); }
	| S_LPARENT expr S_RPARENT				{ $$ = $2; }
	;

arg_list
	: /* */						{ $$ = HZ_NEW(NT_EMPTY); }
	| expr						{ $$ = HZ_NEW(NT_ARG_LIST); $$->addChild($1); }
	| arg_list S_COMMA expr		{ $$ = $1; $$->addChild($3); }
	;

func_call
	: V_IDENT S_LPARENT arg_list S_RPARENT		{	$$ = HZ_NEW(NT_FUNC_CALL);
														auto id = HZ_NEW(V_IDENT);
														id->value = *$1;
														$$->addChild(id);
														$$->addChild($3);
													}
	;

param_list
	: /* */								{ $$ = HZ_NEW(NT_EMPTY); }
	| V_IDENT						{	$$ = HZ_NEW(NT_ARG_LIST);
											auto node = HZ_NEW(V_IDENT);
											node->value = *$1;
											$$->addChild(node);
										}
	| param_list S_COMMA V_IDENT		{	$$ = $1;
											auto node = HZ_NEW(V_IDENT);
											node->value = *$3;
											$$->addChild(node);
										}
	;

param_decl
	: S_LPARENT param_list S_RPARENT		{ $$ = $2; }
	;

func_decl
	: FUNCTION V_IDENT param_decl S_LBRACE block_empty S_RBRACE
		{	$$ = HZ_NEW(NT_FUNC_DECL);
			$$->addChild(V_IDENT, "V_IDENT")->value = *$2;
			$$->addChild($3);
			$$->addChild($5);
		}
	;

attr_decl
	: ATTR V_IDENT		{	$$ = HZ_NEW(NT_ATTR_DECL);
								auto node = HZ_NEW(V_IDENT);
								node->value = *$2;
								$$->addChild(node);
							}
	| ATTR V_IDENT V_IDENT		{	$$ = HZ_NEW(NT_ATTR_DECL);
											auto type = HZ_NEW(V_IDENT);
											type->value = *$2;
											$$->addChild(type);
											auto node = HZ_NEW(V_IDENT);
											node->value = *$2;
											type->addChild(node);
										}
	;

class_body
	: attr_decl				{ $$ = HZ_NEW(NT_CLASS_BODY); $$->addChild($1); }
	| func_decl				{ $$ = HZ_NEW(NT_CLASS_BODY); $$->addChild($1); }
	| class_body attr_decl	{ $$ = $1; $$->addChild($2); }
	| class_body func_decl	{ $$ = $1; $$->addChild($2); }
	;

class_body_decl
	: /* E */			{ $$ = HZ_NEW(NT_EMPTY); }
	| class_body		{ $$ = $1; }
	;

class_decl
	: CLASS V_IDENT S_LBRACE class_body_decl S_RBRACE
		{	$$ = HZ_NEW(NT_CLASS_DECL);
			$$->addChild(V_IDENT, "V_IDENT")->value = *$2;
			$$->addChild($4);
		}
	;

expr_asn_op
	: MATH_ASN				{ $$ = HZ_NEW(MATH_ASN); }
	| MATH_ASN_ADD			{ $$ = HZ_NEW(MATH_ASN_ADD); }
	| MATH_ASN_SUB			{ $$ = HZ_NEW(MATH_ASN_SUB); }
	| MATH_ASN_MUL			{ $$ = HZ_NEW(MATH_ASN_MUL); }
	| MATH_ASN_DIV			{ $$ = HZ_NEW(MATH_ASN_DIV); }
	| MATH_ASN_MOD			{ $$ = HZ_NEW(MATH_ASN_MOD); }
	| MATH_ASN_BITWISE_OR	{ $$ = HZ_NEW(MATH_ASN_BITWISE_OR); }
	| MATH_ASN_BITWISE_AND	{ $$ = HZ_NEW(MATH_ASN_BITWISE_AND); }
	| MATH_ASN_BITWISE_XOR	{ $$ = HZ_NEW(MATH_ASN_BITWISE_XOR); }
	| MATH_ASN_BITWISE_NOT	{ $$ = HZ_NEW(MATH_ASN_BITWISE_NOT); }
	| MATH_ASN_BITWISE_LSH	{ $$ = HZ_NEW(MATH_ASN_BITWISE_LSH); }
	| MATH_ASN_BITWISE_RSH	{ $$ = HZ_NEW(MATH_ASN_BITWISE_RSH); }
	;

binop_all
	: expr_bin_op		{ $$ = $1; }
	| expr_cmp_op		{ $$ = $1; }
	| expr_bit_op		{ $$ = $1; }
	;

expr_bin_op
	: MATH_ADD	{ $$ = HZ_NEW(MATH_ADD); }
	| MATH_SUB	{ $$ = HZ_NEW(MATH_SUB); }
	| MATH_DIV	{ $$ = HZ_NEW(MATH_DIV); }
	| MATH_MUL	{ $$ = HZ_NEW(MATH_MUL); }
	| MATH_MOD	{ $$ = HZ_NEW(MATH_MOD); }
	;

expr_cmp_op
	: LOGIC_EQ		{ $$ = HZ_NEW(LOGIC_EQ); }
	| LOGIC_NEQ		{ $$ = HZ_NEW(LOGIC_NEQ); }
	| LOGIC_NOT		{ $$ = HZ_NEW(LOGIC_NOT); }
	| LOGIC_OR		{ $$ = HZ_NEW(LOGIC_OR); }
	| LOGIC_AND		{ $$ = HZ_NEW(LOGIC_AND); }
	| LOGIC_LT		{ $$ = HZ_NEW(LOGIC_LT); }
	| LOGIC_GT		{ $$ = HZ_NEW(LOGIC_GT); }
	| LOGIC_LET		{ $$ = HZ_NEW(LOGIC_LET); }
	| LOGIC_GET		{ $$ = HZ_NEW(LOGIC_GET); }
	;

expr_bit_op
	: BITWISE_NOT	{ $$ = HZ_NEW(BITWISE_NOT); }
	| BITWISE_OR	{ $$ = HZ_NEW(BITWISE_OR); }
	| BITWISE_AND	{ $$ = HZ_NEW(BITWISE_AND); }
	| BITWISE_XOR	{ $$ = HZ_NEW(BITWISE_XOR); }
	| BITWISE_LSH	{ $$ = HZ_NEW(BITWISE_LSH); }
	| BITWISE_RSH	{ $$ = HZ_NEW(BITWISE_RSH); }
	;

conditional_block
	: IF expr THEN block_empty END			{ $$ = HZ_NEW(IF); $$->addChild($2); $$->addChild($4); }
	;

constant
	: V_NUMBER	{ $$ = HZ_NEW(V_NUMBER); $$->value = $1; }
	| V_STRING	{ $$ = HZ_NEW(V_STRING); $$->value = *$1; }
	| V_TRUE		{ $$ = HZ_NEW(V_TRUE); $$->value = $1; }
	| V_FALSE		{ $$ = HZ_NEW(V_FALSE); $$->value = $1; }
	| V_NIL		{ $$ = HZ_NEW(V_NIL); $$->value = $1; }
	;

variable
	: variable_lval							{ $$ = $1; }
	| variable OP_ACCESSOR variable_lval	{ $$ = HZ_NEW(OP_ACCESSOR); $$->addChild($1); $$->addChild($3); }
	;

variable_lval
	: V_IDENT			{ $$ = HZ_NEW(V_IDENT); $$->value = *$1; }
	| func_call				{ $$ = $1; }
	;