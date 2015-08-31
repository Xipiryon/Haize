%{
#pragma warning(disable: 4065) // switch statement contains 'default' but no 'case' labels

// Generated file will be created in a subfolder called "generated/"
// As we require some files that are not in "include/" folder,
// we must look for them in with a relative path way.
#include "../Extern.hpp"

// Looking for Haize Header, so it's ok
#include <Muon/Type/String.hpp>
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
%left	<node> MATH_ASN
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

%token S_COMMA S_LPARENT S_RPARENT S_LCBRACE S_RCBRACE S_LBRACKET S_RBRACKET

%token <string> IDENTIFIER
%token <string> STRING
%token <floating> NUMBER
%token <integer> TRUE FALSE NIL

%token <node> IF THEN ELSE END FOR WHILE UNLESS IN
%token <node> CLASS ATTR FUNCTION RETURN

/*
* Declares our customs non terminal tokens
*/
%type <node> block chunk stmt_decl expr
%type <node> param_list param_decl func_decl func_body func_call arg_decl arg_list
%type <node> attr_decl class_body class_body_decl class_decl
%type <node> expr_asn_op expr_bin_op expr_cmp_op
%type <node> conditional_block
%type <node> constant variable

/*
* Let's start!
*/
%start program
%%

program
	: /* Empty script is valid, though useless */
	| block				{ g_parseInfo->ASTRoot->addChild($1); }
	;

block
	: chunk				{ $$ = HZ_NEW(NT_BLOCK); $$->addChild($1); }
	| block chunk		{ $$ = $1; $$->addChild($2); }
	;

chunk
	: stmt_decl			{ $$ = HZ_NEW(NT_STMT); $$->addChild($1); }
	| func_decl			{ $$ = $1; }
	| class_decl		{ $$ = $1; }
	;

stmt_decl
	: expr						{ $$ = $1; }
	| RETURN expr				{ $$ = HZ_NEW(RETURN); $$->addChild($2); }
	;

expr
	: constant 							{ $$ = $1; }
	| variable 							{ $$ = $1; }
	| expr_bin_op						{ $$ = $1; }
	| expr_asn_op						{ $$ = $1; }
	| expr_cmp_op						{ $$ = $1; }
	| S_LPARENT expr S_RPARENT			{ $$ = $2; }
	| conditional_block					{ $$ = $1; }
	;

arg_list
	: /* */						{ $$ = HZ_NEW(NT_EMPTY); }
	| expr						{ $$ = HZ_NEW(NT_ARG_LIST); $$->addChild($1); }
	| arg_list S_COMMA expr		{ $$ = $1; $$->addChild($3); }
	;

arg_decl
	: S_LPARENT arg_list S_RPARENT		{ $$ = $2; }
	;

func_call
	: IDENTIFIER arg_decl		{	$$ = HZ_NEW(NT_FUNC_CALL);
									auto id = HZ_NEW(IDENTIFIER);
									id->value = $1;
									$$->addChild(id);
									$$->addChild($2);
								}
	;

param_list
	: /* */								{ $$ = HZ_NEW(NT_EMPTY); }
	| IDENTIFIER						{	$$ = HZ_NEW(NT_ARG_LIST);
											auto node = HZ_NEW(IDENTIFIER);
											node->value = $1;
											$$->addChild(node);
										}
	| param_list S_COMMA IDENTIFIER		{	$$ = $1;
											auto node = HZ_NEW(IDENTIFIER);
											node->value = $3;
											$$->addChild(node);
										}
	;

param_decl
	: S_LPARENT param_list S_RPARENT		{ $$ = $2; }
	;

func_body
	: /* E */			{ $$ = HZ_NEW(NT_EMPTY); }
	| block				{ $$ = $1; }
	;

func_decl
	: FUNCTION IDENTIFIER param_decl func_body END	{	$$ = HZ_NEW(NT_FUNC_DECL);
													$$->addChild(IDENTIFIER, "IDENTIFIER")->value = $2;
													$$->addChild($3);
													$$->addChild($4);
												}
	;

attr_decl
	: ATTR IDENTIFIER		{	$$ = HZ_NEW(NT_ATTR_DECL);
								auto node = HZ_NEW(IDENTIFIER);
								node->value = $2;
								$$->addChild(node);
							}
	;

class_body
	: attr_decl				{ $$ = HZ_NEW(NT_CLASS_BODY); $$->addChild($1); }
	| func_decl				{ $$ = HZ_NEW(NT_CLASS_BODY); $$->addChild($1); }
	| class_body attr_decl	{ $1->addChild($2); }
	| class_body func_decl	{ $1->addChild($2); }
	;

class_body_decl
	: /* E */			{ $$ = HZ_NEW(NT_EMPTY); }
	| class_body		{ $$ = $1; }
	;

class_decl
	: CLASS IDENTIFIER class_body_decl END		{	$$ = HZ_NEW(NT_CLASS_DECL);
													auto id = HZ_NEW(IDENTIFIER);
													id->value = $2;
													$$->addChild(id);
													$$->addChild($3);
												}
	;

expr_asn_op
	: variable MATH_ASN expr				{ $$ = HZ_NEW(MATH_ASN); $$->addChild($1); $$->addChild($3);}
	| variable MATH_ASN_ADD expr			{ $$ = HZ_NEW(MATH_ASN_ADD); $$->addChild($1); $$->addChild($3); }
	| variable MATH_ASN_SUB expr			{ $$ = HZ_NEW(MATH_ASN_SUB); $$->addChild($1); $$->addChild($3); }
	| variable MATH_ASN_MUL expr			{ $$ = HZ_NEW(MATH_ASN_MUL); $$->addChild($1); $$->addChild($3); }
	| variable MATH_ASN_DIV expr			{ $$ = HZ_NEW(MATH_ASN_DIV); $$->addChild($1); $$->addChild($3); }
	| variable MATH_ASN_MOD expr			{ $$ = HZ_NEW(MATH_ASN_MOD); $$->addChild($1); $$->addChild($3); }
	| variable MATH_ASN_BITWISE_OR expr		{ $$ = HZ_NEW(MATH_ASN_BITWISE_OR); $$->addChild($1); $$->addChild($3); }
	| variable MATH_ASN_BITWISE_AND expr	{ $$ = HZ_NEW(MATH_ASN_BITWISE_AND); $$->addChild($1); $$->addChild($3); }
	| variable MATH_ASN_BITWISE_XOR expr	{ $$ = HZ_NEW(MATH_ASN_BITWISE_XOR); $$->addChild($1); $$->addChild($3); }
	| variable MATH_ASN_BITWISE_NOT expr	{ $$ = HZ_NEW(MATH_ASN_BITWISE_NOT); $$->addChild($1); $$->addChild($3); }
	| variable MATH_ASN_BITWISE_LSH expr	{ $$ = HZ_NEW(MATH_ASN_BITWISE_LSH); $$->addChild($1); $$->addChild($3); }
	| variable MATH_ASN_BITWISE_RSH expr	{ $$ = HZ_NEW(MATH_ASN_BITWISE_RSH); $$->addChild($1); $$->addChild($3); }
	;

expr_bin_op
	: expr MATH_ADD expr	{ $$ = HZ_NEW(MATH_ADD); $$->addChild($1); $$->addChild($3); }
	| expr MATH_SUB expr	{ $$ = HZ_NEW(MATH_SUB); $$->addChild($1); $$->addChild($3); }
	| expr MATH_DIV expr	{ $$ = HZ_NEW(MATH_DIV); $$->addChild($1); $$->addChild($3); }
	| expr MATH_MUL expr	{ $$ = HZ_NEW(MATH_MUL); $$->addChild($1); $$->addChild($3); }
	| expr MATH_MOD expr	{ $$ = HZ_NEW(MATH_MOD); $$->addChild($1); $$->addChild($3); }
	;

expr_cmp_op
	: expr_cmp_op LOGIC_EQ expr		{ $$ = HZ_NEW(LOGIC_EQ); $$->addChild($1); $$->addChild($3); }
	;

conditional_block
	: IF S_LPARENT expr_cmp_op S_RPARENT		{ $$ = HZ_NEW(IF); $$->addChild($3); }
	;

constant
	: NUMBER	{ $$ = HZ_NEW(NUMBER); $$->value = $1; }
	| STRING	{ $$ = HZ_NEW(STRING); $$->value = $1; }
	| TRUE		{ $$ = HZ_NEW(TRUE); $$->value = $1; }
	| FALSE		{ $$ = HZ_NEW(FALSE); $$->value = $1; }
	| NIL		{ $$ = HZ_NEW(NIL); $$->value = $1; }
	;

variable
	: IDENTIFIER	{ $$ = HZ_NEW(IDENTIFIER); $$->value = $1; }
	| func_call		{ $$ = $1; }
	;