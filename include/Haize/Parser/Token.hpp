#ifndef INCLUDE_HAIZE_TOKEN_INCLUDED
#define INCLUDE_HAIZE_TOKEN_INCLUDED

#include <Muon/Variant.hpp>
#include "Haize/Core/Define.hpp"

namespace hz
{
	namespace parser
	{
		/*!
		*
		*/
		enum eTokenType
		{
			_EOF = 0,	//!< Private
			_ROOT,		//!<

			S_COMMA,		//!< ','
			S_ACCESSOR,		//!< '.'
			S_SEPARATOR,	//!< ';'
			S_KEYWORD,		//!<
			S_LPARENT,		//!< '('
			S_RPARENT,		//!< ')'
			S_LBRACKET,		//!< '['
			S_RBRACKET,		//!< ']'
			S_LBRACE,		//!< '{'
			S_RBRACE,		//!< '}'

			K_NAMESPACE,
			K_CLASS,
			K_IF,
			K_ELSE,
			K_FOR,
			K_WHILE,
			K_SWITCH,
			K_BREAK,
			K_CONTINUE,
			K_RETURN,
			K_GLOBAL,		//!< Specifier for global context variable
			K_IN,			//!< In specifier for parameter
			K_OUT,			//!< Out specifier for parameter

			V_IDENTIFIER,	//!< An alphanumeric identifier
			V_NUMBER,		//!< A number (in either dec, oct, bin or hex format)
			V_STRING,		//!< A string contained between two '"' character
			V_NIL,			//!<
			V_TRUE,			//!<
			V_FALSE,		//!<

			LOGIC_EQ,		//!< '=='
			LOGIC_NEQ,		//!< '!=' or 'not' keyword
			LOGIC_LT,		//!< '<'
			LOGIC_LET,		//!< '<='
			LOGIC_GT,		//!< '>'
			LOGIC_GET,		//!< '>='
			LOGIC_NOT,		//!< '!'
			LOGIC_AND,		//!< '&&' or 'and' keyword
			LOGIC_OR,		//!< '||' or 'or' keyword

			BITWISE_AND,	//!< '&'
			BITWISE_OR,		//!< '|'
			BITWISE_XOR,	//!< '^'
			BITWISE_NOT,	//!< '~'
			BITWISE_LSH,	//!< '<<'
			BITWISE_RSH,	//!< '>>'

			UNARY_PLUS,		//!< '+' before an expression (is optional: +5 <=> 5)
			UNARY_MINUS,	//!< '-' before an expression (so -5 is not parsed as 0-5)

			MATH_ADD,		//!< '+'
			MATH_SUB,		//!< '-'
			MATH_MUL,		//!< '*'
			MATH_DIV,		//!< '/'
			MATH_MOD,		//!< '%'
			MATH_INC,		//!< '++'
			MATH_DEC,		//!< '--'

			MATH_ASN,		//!< '='
			MATH_ASN_ADD,	//!< '+='
			MATH_ASN_SUB,	//!< '-='
			MATH_ASN_MUL,	//!< '/='
			MATH_ASN_DIV,	//!< '*='
			MATH_ASN_MOD,	//!< '%='
			MATH_ASN_BITWISE_AND,	//!< '&='
			MATH_ASN_BITWISE_OR,	//!< '|='
			MATH_ASN_BITWISE_XOR,	//!< '^='
			MATH_ASN_BITWISE_NOT,	//!< '~='
			MATH_ASN_BITWISE_LSH,	//!< '<<='
			MATH_ASN_BITWISE_RSH,	//!< '>>='

			TOTAL_COUNT		//!< '' Total Count of every stored TokenType,
		};

		/*!
		*
		*/
		static const char* TokenTypeStr[eTokenType::TOTAL_COUNT] =
		{
			"_EOF",
			"_ROOT",

			"S_COMMA",
			"S_ACCESSOR",
			"S_SEPARATOR",
			"S_KEYWORD",
			"S_LPARENT",
			"S_RPARENT",
			"S_LBRACKET",
			"S_RBRACKET",
			"S_LBRACE",
			"S_RBRACE",

			"K_NAMESPACE",
			"K_CLASS",
			"K_IF",
			"K_ELSE",
			"K_FOR",
			"K_WHILE",
			"K_SWITCH",
			"K_BREAK",
			"K_CONTINUE",
			"K_RETURN",
			"K_GLOBAL",
			"K_IN",
			"K_OUT",

			"V_IDENTIFIER",
			"V_NUMBER",
			"V_STRING",
			"V_NIL",
			"V_TRUE",
			"V_FALSE",

			"LOGIC_EQU",
			"LOGIC_DIFF",
			"LOGIC_LT",
			"LOGIC_LET",
			"LOGIC_GT",
			"LOGIC_GET",
			"LOGIC_NOT",
			"LOGIC_AND",
			"LOGIC_OR",

			"BITWISE_AND",
			"BITWISE_OR",
			"BITWISE_XOR",
			"BITWISE_NOT",
			"BITWISE_LSH",
			"BITWISE_RSH",

			"UNARY_PLUS",
			"UNARY_MINUS",

			"MATH_ADD",
			"MATH_SUB",
			"MATH_MUL",
			"MATH_DIV",
			"MATH_MOD",
			"MATH_INC",
			"MATH_DEC",

			"MATH_ASN",
			"MATH_ASN_ADD",
			"MATH_ASN_SUB",
			"MATH_ASN_MUL",
			"MATH_ASN_DIV",
			"MATH_ASN_MOD",
			"MATH_ASN_BITWISE_AND",
			"MATH_ASN_BITWISE_OR",
			"MATH_ASN_BITWISE_XOR",
			"MATH_ASN_BITWISE_NOT",
			"MATH_ASN_BITWISE_LSH",
			"MATH_ASN_BITWISE_RSH",
		};

		/*!
		*
		*/
		struct HAIZE_API Token
		{
			Token()
				: type(_EOF)
				, line(0)
				, column(0)
			{
			}

			Token(eTokenType type_, m::u32 line_, m::u32 column_)
				: type(type_)
				, line(line_)
				, column(column_)
			{
			}

			Token(const Token& t)
			{
				*this = t;
			}

			Token& operator=(const Token& t)
			{
				if (&t != this)
				{
					type = t.type;
					value = t.value;
					line = t.line;
					column = t.column;
				};
				return *this;
			}

			eTokenType type;
			m::Variant value;

			m::u32 line;
			m::u32 column;
		};
	}
}

#endif
