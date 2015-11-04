%option noyywrap nodefault nounistd never-interactive
%option warn nodebug

%{
#pragma warning(disable: 4003) // not enough actual parameters for macro 'yywrap'
#pragma warning(disable: 4005) // macro redefinition (like INT8_MIN and so on...)

// Generated file will be created in a subfolder called "generated/"
// As we require some files that are not in "include/" folder,
// we must look for them in with a relative path way.
#include "../Extern.hpp"
namespace hz
{
	namespace parser
	{
		struct ASTNode;
	}
}

namespace muon
{
	struct String;
}

// And as this file will be generated, it will be in the same folder
// as the current one, so no worries.
#include "yacc.haize.hpp"

// Define our own processInput function,
// so we can send either a file or a buffer, instead of relying upon the
// standard input.
#undef YY_INPUT
#define YY_INPUT(b,r,s) processInput(b,(int*)&r,s)

// We'll use a define, because it's shorter
#define HZ_CHAR g_charCount += yyleng
#define HZ_TOK(token) if(!g_multiLineComment) return token

// Some Utils functions
namespace utils
{
	int hex2int(char* str)
	{
		int val = 0;
		int i = 2; // Skip 0x
		int len = strlen(str);
		for(; i < len; ++i)
		{
			val <<= 4;
			if(str[i] >= '0' && str[i] <= '9')
			{
				val |= str[i] - '0';
			}
			else if(str[i] >= 'a' && str[i] <= 'f')
			{
				val |= str[i] - 'a' + 10;
			}
			else
			{
				val |= str[i] - 'A' + 10;
			}
		}
		return val;
	}

	int oct2int(char* str)
	{
		int val = 0;
		int i = 1; // Skip 0
		int len = strlen(str);
		for(; i < len; ++i)
		{
			val <<= 3;
			val |= str[i] - '0';
		}
		return val;
	}

	int bin2int(char* str)
	{
		int val= 0;
		int i = 2; // Skip 0b
		int len = strlen(str);
		for(; i < len; ++i)
		{
			val <<= 1;
			val |= (str[i] == '1' ? 1 : 0);
		}
		return val;
	}
}

// Now, let's just include the real type
// (include order and all that stuff)
#include <Muon/String.hpp>
#include "Haize/Parser/ASTNode.hpp"

muon::String g_strBuffer;

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
Float		{_IntRange}\.{_IntRange}*f?
Identifer	{_Letters}({_Letters}|{_IntRange})*
%x StringBlock

%%

[ \t]		{ HZ_CHAR; }
{NewLine}	{ ++g_lineCount; g_charCount = 0; }
{Separator}	{ HZ_CHAR; HZ_TOK(S_SEPARATOR); }


<INITIAL>{
	\/\*			{ BEGIN(MultiLineComment); }
}
<MultiLineComment>{
	\*\/			{ BEGIN(INITIAL); }
	[^*\r\n]		{ }
	"*"				{ ++g_charCount; }
	{NewLine}		{ ++g_lineCount; g_charCount = 0; }
}
{LineComment}	{ ++g_lineCount; g_charCount = 0; }


"true"		{ HZ_CHAR; yylval.integer = 1; HZ_TOK(V_TRUE); }
"false"		{ HZ_CHAR; yylval.integer = 0; HZ_TOK(V_FALSE); }
"nil"		{ HZ_CHAR; yylval.integer = 0; HZ_TOK(V_NIL); }

"if"		{ HZ_CHAR; HZ_TOK(IF); }
"else"		{ HZ_CHAR; HZ_TOK(ELSE); }
"elseif"	{ HZ_CHAR; HZ_TOK(ELSEIF); }
"then"		{ HZ_CHAR; HZ_TOK(THEN); }
"for"		{ HZ_CHAR; HZ_TOK(FOR); }
"while"		{ HZ_CHAR; HZ_TOK(WHILE); }
"do"		{ HZ_CHAR; HZ_TOK(DO); }
"in"		{ HZ_CHAR; HZ_TOK(IN); }
"end"		{ HZ_CHAR; HZ_TOK(END); }
"return"	{ HZ_CHAR; HZ_TOK(RETURN); }

"namespace"	{ HZ_CHAR; HZ_TOK(K_NAMESPACE); }
"struct"	{ HZ_CHAR; HZ_TOK(K_STRUCT); }
"attr"		{ HZ_CHAR; HZ_TOK(K_ATTR); }
"function"	{ HZ_CHAR; HZ_TOK(K_FUNCTION); }

{HexNumber}	{
	yylval.integer = utils::hex2int(yytext);
	HZ_CHAR;
	HZ_TOK(V_INTEGER);
}

{OctNumber}	{
	yylval.integer = utils::oct2int(yytext);
	HZ_CHAR;
	HZ_TOK(V_INTEGER);
}

{BinNumber}	{
	yylval.integer = utils::bin2int(yytext);
	HZ_CHAR;
	HZ_TOK(V_INTEGER);
}

{Integer}	{ HZ_CHAR; yylval.integer = atoi(yytext); HZ_TOK(V_INTEGER); }
{Float}		{ HZ_CHAR; yylval.floating = (float)atof(yytext); HZ_TOK(V_FLOATING); }
{Identifer}	{ HZ_CHAR; yylval.string = MUON_CNEW(muon::String, yytext); HZ_TOK(V_IDENT); }

<INITIAL>{
	\"			{ BEGIN(StringBlock); }
}
<StringBlock>{
	\"				{	BEGIN(INITIAL);
						yylval.string = MUON_CNEW(muon::String, g_strBuffer);
						g_strBuffer.clear();
						HZ_TOK(V_STRING);
					}
	[^"\n]+			{ g_strBuffer += yytext;}
	{NewLine}		{ g_strBuffer += yytext; ++g_lineCount; g_charCount = 0; }
}

"."		{ HZ_CHAR; HZ_TOK(OP_ACCESSOR); }
"::"	{ HZ_CHAR; HZ_TOK(OP_NAMESPACE); }
","		{ HZ_CHAR; HZ_TOK(S_COMMA); }
"("		{ HZ_CHAR; HZ_TOK(S_LPARENT); }
")"		{ HZ_CHAR; HZ_TOK(S_RPARENT); }
"{"		{ HZ_CHAR; HZ_TOK(S_LBRACE); }
"}"		{ HZ_CHAR; HZ_TOK(S_RBRACE); }
"["		{ HZ_CHAR; HZ_TOK(S_LBRACKET); }
"]"		{ HZ_CHAR; HZ_TOK(S_RBRACKET); }

"="		{ HZ_CHAR; HZ_TOK(MATH_ASN); }
"+"		{ HZ_CHAR; HZ_TOK(MATH_ADD); }
"-"		{ HZ_CHAR; HZ_TOK(MATH_SUB); }
"*"		{ HZ_CHAR; HZ_TOK(MATH_MUL); }
"/"		{ HZ_CHAR; HZ_TOK(MATH_DIV); }
"%"		{ HZ_CHAR; HZ_TOK(MATH_MOD); }

"+="	{ HZ_CHAR; HZ_TOK(MATH_ASN_ADD); }
"-="	{ HZ_CHAR; HZ_TOK(MATH_ASN_SUB); }
"*="	{ HZ_CHAR; HZ_TOK(MATH_ASN_MUL); }
"/="	{ HZ_CHAR; HZ_TOK(MATH_ASN_DIV); }
"%="	{ HZ_CHAR; HZ_TOK(MATH_ASN_MOD); }

"<"		{ HZ_CHAR; HZ_TOK(LOGIC_LT); }
">"		{ HZ_CHAR; HZ_TOK(LOGIC_GT); }
"<="	{ HZ_CHAR; HZ_TOK(LOGIC_LET); }
">="	{ HZ_CHAR; HZ_TOK(LOGIC_GET); }
"=="	{ HZ_CHAR; HZ_TOK(LOGIC_EQ); }
"!="	{ HZ_CHAR; HZ_TOK(LOGIC_NEQ); }
"&&"	{ HZ_CHAR; HZ_TOK(LOGIC_AND); }
"||"	{ HZ_CHAR; HZ_TOK(LOGIC_OR); }
"!"		{ HZ_CHAR; HZ_TOK(LOGIC_NOT); }

">>"	{ HZ_CHAR; HZ_TOK(BITWISE_RSH); }
"<<"	{ HZ_CHAR; HZ_TOK(BITWISE_LSH); }
">>="	{ HZ_CHAR; HZ_TOK(MATH_ASN_BITWISE_RSH); }
"<<="	{ HZ_CHAR; HZ_TOK(MATH_ASN_BITWISE_LSH); }
"&"		{ HZ_CHAR; HZ_TOK(BITWISE_AND); }
"|"		{ HZ_CHAR; HZ_TOK(BITWISE_OR); }
"~"		{ HZ_CHAR; HZ_TOK(BITWISE_NOT); }
"^"		{ HZ_CHAR; HZ_TOK(BITWISE_XOR); }
"&="	{ HZ_CHAR; HZ_TOK(MATH_ASN_BITWISE_AND); }
"|="	{ HZ_CHAR; HZ_TOK(MATH_ASN_BITWISE_OR); }
"~="	{ HZ_CHAR; HZ_TOK(MATH_ASN_BITWISE_NOT); }
"^="	{ HZ_CHAR; HZ_TOK(MATH_ASN_BITWISE_XOR); }

.		{ HZ_CHAR; }
<<EOF>>	{ yyterminate(); }
