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
* NT_ROOT is used inside cpp code
*/
%token <node> NT_ROOT NT_EMPTY
%token <node> NT_STRUCT_BODY NT_ATTR_DECL
%token <node> NT_FUNC_CALL NT_ARG_LIST
%token <node> NT_STMT

%token S_LPARENT S_RPARENT S_LBRACE S_RBRACE S_LBRACKET S_RBRACKET
%token <node> S_COMMA S_SEPARATOR S_NEWLINE

%token <string> V_IDENT
%token <string> V_STRING
%token <floating> V_FLOATING
%token <integer> V_TRUE V_FALSE V_NIL V_INTEGER

%token <node> IF THEN ELSE ELSEIF END FOR WHILE DO IN RETURN
%token <node> K_NAMESPACE K_STRUCT K_ATTR K_FUNCTION

/*
* Declares our customs non terminal tokens
*/
%type <node> block block_empty chunk chunk_empty stmt_decl expr subexpr
%type <node> namespace_decl
%type <node> param_list param_decl func_decl func_call arg_list
%type <node> attr_decl struct_body struct_body_decl struct_decl
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
	: block				{ $$ = ($1 != NULL ? $1 : HZ_NEW(NT_EMPTY)); }
	| chunk block		{ $$ = $1; if($2 != NULL) { $$->addChild($2); } }
	;

chunk_empty
	: block_empty			{ $$ = ($1 != NULL ? $1 : HZ_NEW(NT_EMPTY)); }
	| chunk block			{ $$ = $1; if($2 != NULL) { $$->addChild($2); } }
	;

block
	: stmt_decl			{ if($1 == NULL) { $$ = NULL; } else { $$ = HZ_NEW(NT_STMT); $$->addChild($1); } }
	| func_decl			{ $$ = $1; }
	| struct_decl		{ $$ = $1; }
	| namespace_decl	{ $$ = $1; }
	| conditional_block 		{ $$ = $1; }
	| RETURN expr S_SEPARATOR	{ $$ = HZ_NEW(RETURN); $$->addChild($2); }
	;

block_empty
	: /* E */			{ $$ = HZ_NEW(NT_EMPTY); }
	| block				{ $$ = $1; }
	;

namespace_decl
	: K_NAMESPACE V_IDENT S_LBRACE chunk_empty S_RBRACE	{ $$ = HZ_NEW(K_NAMESPACE); $$->value = *$2; $$->addChild($4); }
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
	: V_IDENT S_LPARENT arg_list S_RPARENT
		{	$$ = HZ_NEW(NT_FUNC_CALL);
			auto id = HZ_NEW(V_IDENT);
			id->value = *$1;
			$$->addChild(id);
			$$->addChild($3);
		}
	;

param_list
	: /* */		{ $$ = HZ_NEW(NT_EMPTY); }
	| V_IDENT
		{	$$ = HZ_NEW(NT_ARG_LIST);
			auto node = HZ_NEW(V_IDENT);
			node->value = *$1;
			$$->addChild(node);
		}
	| param_list S_COMMA V_IDENT
		{	$$ = $1;
			auto node = HZ_NEW(V_IDENT);
			node->value = *$3;
			$$->addChild(node);
		}
	;

param_decl
	: S_LPARENT param_list S_RPARENT		{ $$ = $2; }
	;

func_decl
	: K_FUNCTION V_IDENT param_decl S_LBRACE block_empty S_RBRACE
		{	$$ = HZ_NEW(K_FUNCTION);
			$$->addChild(V_IDENT, "V_IDENT")->value = *$2;
			$$->addChild($3);
			$$->addChild($5);
		}
	;

attr_decl
	: K_ATTR V_IDENT
		{	$$ = HZ_NEW(NT_ATTR_DECL);
			auto node = HZ_NEW(V_IDENT);
			node->value = *$2;
			$$->addChild(node);
		}
	| K_ATTR V_IDENT V_IDENT
		{	$$ = HZ_NEW(NT_ATTR_DECL);
			auto type = HZ_NEW(V_IDENT);
			type->value = *$2;
			$$->addChild(type);
			auto node = HZ_NEW(V_IDENT);
			node->value = *$2;
			type->addChild(node);
		}
	;

struct_body
	: attr_decl				{ $$ = HZ_NEW(NT_STRUCT_BODY); $$->addChild($1); }
	| func_decl				{ $$ = HZ_NEW(NT_STRUCT_BODY); $$->addChild($1); }
	| struct_body attr_decl	{ $$ = $1; $$->addChild($2); }
	| struct_body func_decl	{ $$ = $1; $$->addChild($2); }
	;

struct_body_decl
	: /* E */			{ $$ = HZ_NEW(NT_EMPTY); }
	| struct_body		{ $$ = $1; }
	;

struct_decl
	: K_STRUCT V_IDENT S_LBRACE struct_body_decl S_RBRACE
		{	$$ = HZ_NEW(K_STRUCT);
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
	: V_INTEGER		{ $$ = HZ_NEW(V_INTEGER); $$->value.set<muon::u32>($1); }
	| V_FLOATING	{ $$ = HZ_NEW(V_FLOATING); $$->value.set<muon::f32>($1); }
	| V_STRING		{ $$ = HZ_NEW(V_STRING); $$->value = *$1; }
	| V_TRUE		{ $$ = HZ_NEW(V_TRUE); $$->value = true; }
	| V_FALSE		{ $$ = HZ_NEW(V_FALSE); $$->value = false; }
	| V_NIL			{ $$ = HZ_NEW(V_NIL); $$->value.reset(); }
	;

variable
	: variable_lval							{ $$ = $1; }
	| variable OP_ACCESSOR variable_lval	{ $$ = HZ_NEW(OP_ACCESSOR); $$->addChild($1); $$->addChild($3); }
	;

variable_lval
	: V_IDENT			{ $$ = HZ_NEW(V_IDENT); $$->value = *$1; }
	| func_call				{ $$ = $1; }
	;