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
* Haize/Parser/Token.hpp mirrors the declaration of 
* the following tokens (Terminals and Non-Terminals)
*/
%token NT_ROOT NT_BLOCK NT_CHUNK NT_EMPTY
%token NT_CLASS_DECL NT_ATTR_DECL
%token NT_FUNC_DECL NT_ARG_LIST 
%token NT_STMT 

%token S_COMMA S_LPARENT S_RPARENT S_LCBRACE S_RCBRACE S_LBRACKET S_RBRACKET

%token <string> IDENTIFIER
%token <string> STRING
%token <floating> NUMBER
%token <integer> TRUE FALSE NIL

%token <node> IF THEN ELSE END FOR WHILE
%token <node> CLASS ATTR FUNCTION RETURN

/* 
* Operator and Token Precedence
*/
%left	MATH_ASN
		MATH_ASN_ADD MATH_ASN_SUB MATH_ASN_MUL MATH_ASN_DIV MATH_ASN_MOD
		MATH_ASN_BITWISE_OR MATH_ASN_BITWISE_AND
		MATH_ASN_BITWISE_XOR MATH_ASN_BITWISE_NOT
		MATH_ASN_BITWISE_LSH MATH_ASN_BITWISE_RSH
%left	LOGIC_OR
%left	LOGIC_AND
%left	BITWISE_OR
%left	BITWISE_XOR
%left	BITWISE_AND
%left	LOGIC_EQ LOGIC_NEQ
%left	LOGIC_LT LOGIC_GT LOGIC_LET LOGIC_GET
%left	BITWISE_LSH BITWISE_RSH
%left	MATH_ADD MATH_SUB
%left	MATH_MUL MATH_DIV MATH_MOD
%right	MATH_PREFINC MATH_PREFDEC LOGIC_NOT BITWISE_NOT UNARY_SIGN
%left	MATH_POSTINC MATH_POSTDEC OP_ACCESSOR
%left	OP_NAMESPACE

/* 
* Declares our customs non terminal tokens
*/
%type <node> block 
%type <node> chunk
%type <node> stmt_decl

%type <node> arg_list
%type <node> arg_decl
%type <node> func_body
%type <node> func_decl

%type <node> attr_decl
%type <node> class_body
%type <node> class_body_decl
%type <node> class_decl

%type <node> rvalue
%type <node> lvalue

/*
* Let's start!
*/
%start program
%%

program:
	| block				{ g_parseInfo->ASTRoot->addChild($1); }
	;
	
block:
	chunk				{ $$ = HZ_NEW(NT_BLOCK); $$->addChild($1); }
	| block chunk		{ $$ = $1; $$->addChild($2); }
	;

chunk:
	stmt_decl			{ $$ = HZ_NEW(NT_STMT); $$->addChild($1); }
	| func_decl			{ $$ = $1; }
	| class_decl		{ $$ = $1; }
	;

stmt_decl:
	rvalue					{ $$ = $1; }
	| RETURN rvalue			{ $$ = HZ_NEW(RETURN); $$->addChild($2); }
	;

arg_list:
	/* */								{ $$ = HZ_NEW(NT_EMPTY); }
	| IDENTIFIER						{	$$ = HZ_NEW(NT_ARG_LIST); 
											auto node = HZ_NEW(IDENTIFIER);
											node->value = $1;
											$$->addChild(node);
										}
	| arg_list S_COMMA IDENTIFIER		{	$$ = $1;
											auto node = HZ_NEW(IDENTIFIER);
											node->value = $3;
											$$->addChild(node);
										}
	;

arg_decl:
	S_LPARENT arg_list S_RPARENT		{ $$ = $2; }
	;

func_body:
	/* */					{ $$ = HZ_NEW(NT_EMPTY); }
	| block					{ $$ = $1; }
	;

func_decl:
	FUNCTION IDENTIFIER arg_decl func_body END	{	$$ = HZ_NEW(NT_FUNC_DECL); 
													$$->addChild(IDENTIFIER, "IDENTIFIER")->value = $2;
													$$->addChild($3);
													$$->addChild($4);
												}
	;

attr_decl:
	ATTR IDENTIFIER			{	$$ = HZ_NEW(NT_ATTR_DECL);
								auto node = HZ_NEW(IDENTIFIER);
								node->value = $2;
								$$->addChild(node);
							}
	;

class_body:
	attr_decl				{ $$->addChild($1); }
	| func_decl				{ $$->addChild($1); }
	| class_body attr_decl	{ $1->addChild($2); }
	| class_body func_decl	{ $1->addChild($2); }
	;

class_body_decl:
	/* */				{ $$ = HZ_NEW(NT_EMPTY); }
	| class_body		{ $$ = $1; }
	;

class_decl:
	CLASS IDENTIFIER class_body_decl END			{ }
	;

rvalue:
	NUMBER		{ $$ = HZ_NEW(NUMBER); $$->value = $1; }
	| STRING	{ $$ = HZ_NEW(STRING); $$->value = $1; }
	| TRUE		{ $$ = HZ_NEW(TRUE); $$->value = $1; }
	| FALSE		{ $$ = HZ_NEW(FALSE); $$->value = $1; }
	| NIL		{ $$ = HZ_NEW(NIL); $$->value = $1; }
	| lvalue	{ $$ = $1}
	;

lvalue:
	IDENTIFIER	{ $$ = HZ_NEW(IDENTIFIER); $$->value = $1; }
	;