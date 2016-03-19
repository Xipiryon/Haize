%option reentrant
%option noyywrap nodefault nounistd never-interactive
%option warn nodebug

%option extra-type="void*"

%{
#pragma warning(disable: 4003) // not enough actual parameters for macro 'yywrap'
#pragma warning(disable: 4005) // macro redefinition (like INT8_MIN and so on...)

namespace utils
{
	int hex2int(char* str);
	int oct2int(char* str);
	int bin2int(char* str);
}

#include <Muon/String.hpp>
#include "Haize/Parser/Token.hpp"
#include "Haize/Parser/ASTNode.hpp"
#include "yacc.hpp"

#define YYEXTRA_HZ_TOKEN ((hz::parser::Token*)yyextra)
%}

_Letters	[a-zA-Z_]

NewLine		[\r\n]
Separator 	[;]

LineComment			\/\/.*{NewLine}
%x MultiLineComment

HexNumber	0[xX][0-9a-fA-F]+
OctNumber	0[0-7]+
BinNumber	0[bB][0-1]+

_IntRange	[0-9]
Integer		[1-9]{_IntRange}*|0
Identifer	{_Letters}({_Letters}|{_IntRange})*
Float		{_IntRange}+\.{_IntRange}*f?

%x StringBlock

%%

[[:blank:]]	{ YYEXTRA_HZ_TOKEN->column += yyleng; }
{NewLine}	{ ++YYEXTRA_HZ_TOKEN->line; YYEXTRA_HZ_TOKEN->column = 1; }
{Separator}	{ return S_SEPARATOR; }


<INITIAL>{
	\/\*			{ BEGIN(MultiLineComment); YYEXTRA_HZ_TOKEN->column += yyleng; }
}
<MultiLineComment>{
	\*\/			{ BEGIN(INITIAL); YYEXTRA_HZ_TOKEN->column += yyleng; }
	[^*\r\n]		{ YYEXTRA_HZ_TOKEN->column += yyleng; }
	"*"				{ YYEXTRA_HZ_TOKEN->column += yyleng; }
	{NewLine}		{ ++YYEXTRA_HZ_TOKEN->line; YYEXTRA_HZ_TOKEN->column = 1; }
}
{LineComment}	{ ++YYEXTRA_HZ_TOKEN->line; YYEXTRA_HZ_TOKEN->column = 1; }


"true"		{ return V_TRUE; }
"false"		{ return V_FALSE; }
"nil"		{ return V_NIL; }

"if"		{ return K_IF; }
"else"		{ return K_ELSE; }
"for"		{ return K_FOR; }
"while"		{ return K_WHILE; }
"switch"	{ return K_SWITCH; }
"break"		{ return K_BREAK; }
"continue"	{ return K_CONTINUE; }
"return"	{ return K_RETURN; }

"global"	{ return K_GLOBAL; }

"namespace"	{ return K_NAMESPACE; }
"class"		{ return K_CLASS; }
"in"		{ return K_IN; }
"out"		{ return K_OUT; }

{HexNumber}	{
	YYEXTRA_HZ_TOKEN->value = utils::hex2int(yytext);
	YYEXTRA_HZ_TOKEN->column += yyleng;
	return V_NUMBER;
}

{OctNumber}	{
	YYEXTRA_HZ_TOKEN->value = utils::oct2int(yytext);
	YYEXTRA_HZ_TOKEN->column += yyleng;
	return V_NUMBER;
}

{BinNumber}	{
	YYEXTRA_HZ_TOKEN->value = utils::bin2int(yytext);
	YYEXTRA_HZ_TOKEN->column += yyleng;
	return V_NUMBER;
}

{Integer}	{ YYEXTRA_HZ_TOKEN->value = (m::i32)atoi(yytext);	return V_NUMBER; }
{Float}		{ YYEXTRA_HZ_TOKEN->value = (m::f32)atof(yytext);	return V_NUMBER; }
{Identifer}	{ YYEXTRA_HZ_TOKEN->value = m::String(yytext);		return V_IDENTIFIER; }

<INITIAL>{
	\"			{ BEGIN(StringBlock); YYEXTRA_HZ_TOKEN->column += yyleng; }
}
<StringBlock>{
	\"				{	BEGIN(INITIAL);
						//yylval.string = MUON_CNEW(muon::String, g_strBuffer);
						//g_strBuffer.clear();
						return V_STRING;
					}
	[^"\n]+			{ /*g_strBuffer += yytext;*/ YYEXTRA_HZ_TOKEN->column += yyleng;}
	{NewLine}		{ /*g_strBuffer += yytext;*/ ++YYEXTRA_HZ_TOKEN->line; YYEXTRA_HZ_TOKEN->column = 1; }
}

"."		{ return S_ACCESSOR; }
","		{ return S_COMMA; }
"("		{ return S_LPARENT; }
")"		{ return S_RPARENT; }
"{"		{ return S_LBRACE; }
"}"		{ return S_RBRACE; }
"["		{ return S_LBRACKET; }
"]"		{ return S_RBRACKET; }

"="		{ return MATH_ASN; }
"+"		{ return MATH_ADD; }
"-"		{ return MATH_SUB; }
"*"		{ return MATH_MUL; }
"/"		{ return MATH_DIV; }
"%"		{ return MATH_MOD; }

"+="	{ return MATH_ASN_ADD; }
"-="	{ return MATH_ASN_SUB; }
"*="	{ return MATH_ASN_MUL; }
"/="	{ return MATH_ASN_DIV; }
"%="	{ return MATH_ASN_MOD; }

"<"		{ return LOGIC_LT; }
">"		{ return LOGIC_GT; }
"<="	{ return LOGIC_LET; }
">="	{ return LOGIC_GET; }
"=="	{ return LOGIC_EQ; }
"!="	{ return LOGIC_NEQ; }
"&&"	{ return LOGIC_AND; }
"||"	{ return LOGIC_OR; }
"!"		{ return LOGIC_NOT; }

">>"	{ return BITWISE_RSH; }
"<<"	{ return BITWISE_LSH; }
"&"		{ return BITWISE_AND; }
"|"		{ return BITWISE_OR; }
"~"		{ return BITWISE_NOT; }
"^"		{ return BITWISE_XOR; }
"&="	{ return MATH_ASN_BITWISE_AND; }
"|="	{ return MATH_ASN_BITWISE_OR; }
"^="	{ return MATH_ASN_BITWISE_XOR; }
"~="	{ return MATH_ASN_BITWISE_NOT; }
">>="	{ return MATH_ASN_BITWISE_RSH; }
"<<="	{ return MATH_ASN_BITWISE_LSH; }

.		{ YYEXTRA_HZ_TOKEN->column += yyleng; }
<<EOF>>	{ yyterminate(); }
