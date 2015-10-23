#ifndef _HAIZE_STRUCTS_INCLUDE
#define _HAIZE_STRUCTS_INCLUDE

#include <deque>
#include <Muon/Variant.hpp>
#include "Haize/Core/Define.hpp"
#include "Haize/VM/OpCode.hpp"

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

			S_COMMA,		//!< ','
			S_ACCESSOR,		//!< '.'
			S_RESOLUTION,	//!< '::'
			S_SEPARATOR,	//!< New line, or ';'
			S_KEYWORD,		//!<
			S_LPARENT,		//!< '('
			S_RPARENT,		//!< ')'
			S_LBRACKET,		//!< '['
			S_RBRACKET,		//!< ']'
			S_LBRACE,		//!< '{'
			S_RBRACE,		//!< '}'

			V_NUMBER,		//!< A number (in either dec, oct, bin or hex format)
			V_IDENTIFIER,	//!< An alphanumeric identifier
			V_STRING,		//!< A string contained between two '"' character
			V_NIL,			//!<
			V_TRUE,			//!<
			V_FALSE,		//!<

			E_LOGIC_OP_BEGIN,
			LOGIC_EQ,		//!< '=='
			LOGIC_NEQ,		//!< '!=' or 'not' keyword
			LOGIC_LT,		//!< '<'
			LOGIC_LET,		//!< '<='
			LOGIC_GT,		//!< '>'
			LOGIC_GET,		//!< '>='
			LOGIC_NOT,		//!< '!'
			LOGIC_AND,		//!< '&&' or 'and' keyword
			LOGIC_OR,		//!< '||' or 'or' keyword
			E_LOGIC_OP_END,

			E_BITWISE_OP_BEGIN,
			BITWISE_AND,	//!< '&'
			BITWISE_OR,		//!< '|'
			BITWISE_XOR,	//!< '^'
			BITWISE_NOT,	//!< '~'
			BITWISE_LSH,	//!< '<<'
			BITWISE_RSH,	//!< '>>'
			E_BITWISE_OP_END,

			UNARY_PLUS,		//!< '+' before an expression (is optional: +5 <=> 5)
			UNARY_MINUS,	//!< '-' before an expression (-5 is not parsed as 0-5)

			E_MATH_OP_BEGIN,
			MATH_ADD,		//!< '+'
			MATH_SUB,		//!< '-'
			MATH_MUL,		//!< '*'
			MATH_DIV,		//!< '/'
			MATH_MOD,		//!< '%'
			MATH_INC,		//!< '++'
			MATH_DEC,		//!< '--'
			E_MATH_OP_END,

			E_ASN_OP_BEGIN,
			MATH_ASN,		//!< '='
			MATH_ASN_ADD,	//!< '+='
			MATH_ASN_SUB,	//!< '-='
			MATH_ASN_MUL,	//!< '/='
			MATH_ASN_DIV,	//!< '*='
			MATH_ASN_MOD,	//!< '%='
			MATH_ASN_AND,	//!< '&='
			MATH_ASN_OR,	//!< '|='
			MATH_ASN_XOR,	//!< '^='
			MATH_ASN_NOT,	//!< '~='
			E_ASN_OP_END,

			NT_CHUNK,
			NT_BLOCK,
			NT_BLOCK_EMPTY,
			NT_STMT_DECL,
			NT_EXPR,
			NT_PREFIXEXPR,
			NT_ARG_LIST,
			NT_FUNC_CALL,
			NT_PARAM_DECL,
			NT_PARAM_LIST,
			NT_FUNC_DECL,
			NT_ATTR_DECL,
			NT_CLASS_BODY,
			NT_CLASS_BODY_DECL,
			NT_CLASS_DECL,
			NT_OP_ASN,
			NT_OP_BITW,
			NT_OP_MATH,
			NT_OP_CMP,
			NT_CONDITION_BLOCK,
			CATEGORY_BINOP,
			CATEGORY_UNOP,
			CATEGORY_CONSTANT,
			NT_VARIABLE,
			NT_VARIABLE_LVAL,
			NT_RETURN,

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
			"S_RESOLUTION",
			"S_SEPARATOR",
			"S_KEYWORD",
			"S_LPARENT",
			"S_RPARENT",
			"S_LBRACKET",
			"S_RBRACKET",
			"S_LBRACE",
			"S_RBRACE",

			"V_NUMBER",
			"V_IDENTIFIER",
			"V_STRING",
			"V_NIL",
			"V_TRUE",
			"V_FALSE",

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
			"MATH_INC",
			"MATH_DEC",
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

			"NT_CHUNK",
			"NT_BLOCK",
			"NT_BLOCK_EMPTY",
			"NT_STMT_DECL",
			"NT_EXPR",
			"NT_PREFIXEXPR",
			"NT_ARG_LIST",
			"NT_FUNC_CALL",
			"NT_PARAM_DECL",
			"NT_PARAM_LIST",
			"NT_FUNC_DECL",
			"NT_ATTR_DECL",
			"NT_CLASS_BODY",
			"NT_CLASS_BODY_DECL",
			"NT_CLASS_DECL",
			"NT_OP_ASN",
			"NT_OP_BITW",
			"NT_OP_MATH",
			"NT_OP_CMP",
			"NT_CONDITION_BLOCK",
			"CATEGORY_BINOP",
			"CATEGORY_UNOP",
			"CATEGORY_CONSTANT",
			"NT_VARIABLE",
			"NT_VARIABLE_LVAL",
			"NT_RETURN",
		};

		/*!
		*
		*/
		struct HAIZE_API Token
		{
			Token();
			Token(const Token&);
			explicit Token(eTokenType);
			~Token();
			Token& operator=(const Token&);

			eTokenType type;
			eTokenType category;
			muon::Variant value;

			muon::String section;
			muon::String function;
			muon::u32 line;
			muon::u32 column;
		};
	}
}

#endif
