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
			S_INVALID = 0,
			S_EOF,

			S_COMMA,			//!< ','
			S_ACCESSOR,			//!< '.'
			S_SEPARATOR,		//!< ';'
			S_KEYWORD,			//!<
			S_LPARENT,			//!< '('
			S_RPARENT,			//!< ')'
			S_LBRACKET,			//!< '['
			S_RBRACKET,			//!< ']'
			S_LBRACE,			//!< '{'
			S_RBRACE,			//!< '}'

			V_IDENTIFIER,		//!< An alphanumeric identifier
			E_CONSTANT_BEGIN,
			V_NUMBER,			//!< A number (in either dec, oct, bin or hex format)
			V_STRING,			//!< A string contained between two '"' character
			V_NIL,				//!<
			V_TRUE,				//!<
			V_FALSE,			//!<
			E_CONSTANT_END,

			E_LOGIC_OP_BEGIN,
			LOGIC_EQ,			//!< '=='
			LOGIC_NEQ,			//!< '!=' or 'not' keyword
			LOGIC_LT,			//!< '<'
			LOGIC_LET,			//!< '<='
			LOGIC_GT,			//!< '>'
			LOGIC_GET,			//!< '>='
			LOGIC_NOT,			//!< '!'
			LOGIC_AND,			//!< '&&' or 'and' keyword
			LOGIC_OR,			//!< '||' or 'or' keyword
			E_LOGIC_OP_END,

			E_BITWISE_OP_BEGIN,
			BITWISE_AND,		//!< '&'
			BITWISE_OR,			//!< '|'
			BITWISE_XOR,		//!< '^'
			BITWISE_NOT,		//!< '~'
			BITWISE_LSH,		//!< '<<'
			BITWISE_RSH,		//!< '>>'
			E_BITWISE_OP_END,

			UNARY_PLUS,			//!< '+' before an expression (is optional: +5 <=> 5)
			UNARY_MINUS,		//!< '-' before an expression (so -5 is not parsed as 0-5)

			E_MATH_OP_BEGIN,
			MATH_ADD,			//!< '+'
			MATH_SUB,			//!< '-'
			MATH_MUL,			//!< '*'
			MATH_DIV,			//!< '/'
			MATH_MOD,			//!< '%'
			MATH_INC_DEFAULT,	//<! Only used by the parser, '++'
			MATH_DEC_DEFAULT,	//<! Only used by the parser, '--'
			MATH_INC_POSTFIX,	//!< var '++'
			MATH_DEC_POSTFIX,	//!< var '--'
			MATH_INC_PREFIX,	//!< '++' var
			MATH_DEC_PREFIX,	//!< '--' var
			E_MATH_OP_END,

			E_ASN_OP_BEGIN,
			MATH_ASN,			//!< '='
			MATH_ASN_ADD,		//!< '+='
			MATH_ASN_SUB,		//!< '-='
			MATH_ASN_MUL,		//!< '/='
			MATH_ASN_DIV,		//!< '*='
			MATH_ASN_MOD,		//!< '%='
			MATH_ASN_AND,		//!< '&='
			MATH_ASN_OR,		//!< '|='
			MATH_ASN_XOR,		//!< '^='
			MATH_ASN_NOT,		//!< '~='
			E_ASN_OP_END,

			E_TERMINALTOKEN_END,

			NT_ROOT,
			NT_FUNCTION_DECL,
			NT_CLASS_DECL,
			NT_CLASS_MEMBER,
			NT_CLASS_CONSTRUCTOR,
			NT_CLASS_DESTRUCTOR,
			NT_GLOBAL_DECL,
			NT_NAMESPACE_DECL,
			NT_EXPR,
			NT_EXPR_NEW,
			NT_EXPR_DELETE,
			NT_EXPR_FUNC_CALL,
			NT_EXPR_IF,
			NT_EXPR_FOR,
			NT_EXPR_WHILE,
			NT_EXPR_SWITCH,
			NT_EXPR_CASE,
			NT_EXPR_CONTINUE,
			NT_EXPR_BREAK,
			NT_EXPR_RETURN,

			TOTAL_COUNT		//!< '' Total Count of every stored TokenType,
		};

		/*!
		*
		*/
		static const char* TokenTypeStr[eTokenType::TOTAL_COUNT] =
		{
			"S_INVALID",
			"S_EOF",

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

			"V_IDENTIFIER",
			"E_CONSTANT_BEGIN",
			"V_NUMBER",
			"V_STRING",
			"V_NIL",
			"V_TRUE",
			"V_FALSE",
			"E_CONSTANT_END",

			"",
			"LOGIC_EQU",
			"LOGIC_DIFF",
			"LOGIC_LT",
			"LOGIC_LET",
			"LOGIC_GT",
			"LOGIC_GET",
			"LOGIC_NOT",
			"LOGIC_AND",
			"LOGIC_OR",
			"",

			"",
			"BITWISE_AND",
			"BITWISE_OR",
			"BITWISE_XOR",
			"BITWISE_NOT",
			"BITWISE_LSH",
			"BITWISE_RSH",
			"",

			"UNARY_PLUS",
			"UNARY_MINUS",

			"",
			"MATH_ADD",
			"MATH_SUB",
			"MATH_MUL",
			"MATH_DIV",
			"MATH_MOD",
			"MATH_INC_DEFAULT",
			"MATH_DEC_DEFAULT",
			"MATH_INC_POSTFIX",
			"MATH_DEC_POSTFIX",
			"MATH_INC_PREFIX",
			"MATH_DEC_PREFIX",
			"",

			"",
			"MATH_ASN",
			"MATH_ASN_ADD",
			"MATH_ASN_SUB",
			"MATH_ASN_MUL",
			"MATH_ASN_DIV",
			"MATH_ASN_MOD",
			"MATH_ASN_AND",
			"MATH_ASN_OR",
			"MATH_ASN_XOR",
			"MATH_ASN_NOT",
			"",

			"",

			"NT_ROOT",
			"NT_FUNCTION_DECL",
			"NT_CLASS_DECL",
			"NT_CLASS_MEMBER",
			"NT_CLASS_CONSTRUCTOR",
			"NT_CLASS_DESTRUCTOR",
			"NT_GLOBAL_DECL",
			"NT_NAMESPACE_DECL",
			"NT_EXPR",
			"NT_EXPR_NEW",
			"NT_EXPR_DELETE",
			"NT_EXPR_FUNC_CALL",
			"NT_EXPR_IF",
			"NT_EXPR_FOR",
			"NT_EXPR_WHILE",
			"NT_EXPR_SWITCH",
			"NT_EXPR_CASE",
			"NT_EXPR_CONTINUE",
			"NT_EXPR_BREAK",
			"NT_EXPR_RETURN",
		};

		/*!
		*
		*/
		struct HAIZE_API Token
		{
			Token()
				: type(S_INVALID)
				, line(0)
				, column(0)
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
					section = t.section;
					line = t.line;
					column = t.column;
				};
				return *this;
			}

			m::String valueToStr() const
			{
				char buffer[32];
				m::String tokstr;
				m::meta::MetaData* m = value.getMeta();
				if (MUON_META(m::String) == m)
				{
					tokstr = value.get<m::String>();
				}
				else if (MUON_META(m::f32) == m)
				{
					m::ftoa(value.get<m::f32>(), buffer);
					tokstr = buffer;
				}
				else if (MUON_META(m::i32) == m)
				{
					m::itoa((m::i64)value.get<m::i32>(), buffer);
					tokstr = buffer;
				}
				else
				{
					tokstr = TokenTypeStr[type];
				}
				return tokstr;
			}

			eTokenType type;
			m::Variant value;

			m::u32 section;
			m::u32 line;
			m::u32 column;
		};
	}
}

#endif
