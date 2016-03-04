
#include <Muon/System/Log.hpp>

#include "Haize/Context.hpp"

namespace
{
	enum OpAssociativity
	{
		ASSOC_LEFT,
		ASSOC_RIGHT,
	};

	struct OpAttribute
	{
		m::u32 precedence;
		OpAssociativity associativity;
	};
	static const OpAttribute s_OpDefault = { 0, ASSOC_LEFT };

	enum ParserPhase
	{
		PHASE_START,
		PHASE_EXPRESSION,
		PHASE_CLASS,
		PHASE_FUNCTION,
	};

	/*
	* Custom parser::InfoImpl
	* Specific for Lexical parsing
	*/
	struct InfoSyntaxic : public hz::parser::InfoImpl
	{
		virtual ~InfoSyntaxic()
		{
		}

		std::deque<ParserPhase> phases;
		m::u32 readIndex;

		std::deque<hz::parser::Token> operatorToken;
		std::deque<hz::parser::ASTNode*> nodes;

		static const OpAttribute precAttrib[hz::parser::E_TERMINALTOKEN_END];
	};
}

namespace hz
{
	bool Context::parseSyntaxic()
	{
		while (!m_nodeRoot->children->empty())
		{
			MUON_DELETE(m_nodeRoot->children->back());
		}

		// Reinit token
		m_nodeRoot->name = "#ERROR#";
		m_nodeRoot->token = parser::Token(parser::TOTAL_COUNT);

		InfoSyntaxic impl;
		impl.phases.push_back(PHASE_START);
		impl.readIndex = 0;

		if (m_tokenList->empty() || m_tokenList->back().type == parser::S_EOF)
		{
			clearError(false);
			m_error.message = "Empty token list, nothing to parse.";
			return false;
		}

		bool err = false;
		hz::parser::ASTNode* node = NULL;

		while(!m_tokenList->empty())
		{
			parser::Token currToken = m_tokenList->back();
			if(currToken.type == parser::S_KEYWORD)
			{
				// Class or Function
				m::String keyword = currToken.value.get<m::String>();
				if (keyword == "class")
				{
					err = parseClass(&impl);
				}
				else if (keyword == "func")
				{
					err = parseFunction(&impl);
				}
				else if (keyword == "global")
				{
					err = parseGlobal(&impl);
				}
				else
				{
					clearError(false);
					m_error.line = currToken.line;
					m_error.column = currToken.column;
					m_error.message = "Unexpected keyword \"" + keyword + "\"";
					return false;
				}
			}
			else
			{
				clearError(false);
				m_error.line = currToken.line;
				m_error.column = currToken.column;
				m_error.message = "Code can't be outside functions/class or must have \"global\" specifier";
				return false;
			}

			if(err)
			{
				// Error should have been set in parse{...} functions
				return false;
			}
		}

		clearError(true);
		return true;
	}

	bool Context::parseExpression(parser::InfoImpl* info)
	{
		InfoSyntaxic* impl = (InfoSyntaxic*)info;
		parser::Token currToken = m_tokenList->back();
		if (currToken.category == parser::CATEGORY_BINOP)
		{
			// If we've less than 2 variable on the left, there is a problem
			if (m_tokenList->size() < 2)
			{
				clearError(false);
				m_error.line = currToken.line;
				m_error.column = currToken.column;
				m_error.message = "Operator has not enough operand!";
				return false;
			}
			// old code
			//node = INFO_IMPL->stackValue[i];
			//node->addChild(INFO_IMPL->stackValue[i - 2]);
			//node->addChild(INFO_IMPL->stackValue[i - 1]);
			//INFO_IMPL->stackValue.erase(INFO_IMPL->stackValue.begin() + (--i)); // erase right
			//INFO_IMPL->stackValue.erase(INFO_IMPL->stackValue.begin() + (--i)); // erase left
			// */
		}
		m_tokenList->pop_back();
		return true;
	}

	bool Context::parseGlobal(parser::InfoImpl* info)
	{
		InfoSyntaxic* impl = (InfoSyntaxic*)info;
		return true;
	}

	bool Context::parseClass(parser::InfoImpl* info)
	{
		InfoSyntaxic* impl = (InfoSyntaxic*)info;
		return true;
	}

	bool Context::parseFunction(parser::InfoImpl* info)
	{
		InfoSyntaxic* impl = (InfoSyntaxic*)info;
		return true;
	}
}

const OpAttribute InfoSyntaxic::precAttrib[hz::parser::E_TERMINALTOKEN_END] =
{
	s_OpDefault, //S_INVALID,
	s_OpDefault, //S_EOF,

	s_OpDefault, //S_COMMA,			','
	{ 85, ASSOC_RIGHT },	//S_ACCESSOR,		'.'
	{ 90, ASSOC_LEFT },		//S_RESOLUTION,		'::'
	s_OpDefault, //S_SEPARATOR,		New line, or ';'
	s_OpDefault, //S_KEYWORD,
	s_OpDefault, //S_LPARENT,		'('
	s_OpDefault, //S_RPARENT,		')'
	s_OpDefault, //S_LBRACKET,		'['
	s_OpDefault, //S_RBRACKET,		']'
	s_OpDefault, //S_LBRACE,		'{'
	s_OpDefault, //S_RBRACE,		'}'

	s_OpDefault, //V_NUMBER,			A number (in either dec, oct, bin or hex format)
	s_OpDefault, //V_IDENTIFIER,		An alphanumeric identifier
	s_OpDefault, //V_STRING,			A string contained between two '"' character
	s_OpDefault, //V_NIL,
	s_OpDefault, //V_TRUE,
	s_OpDefault, //V_FALSE,

	s_OpDefault, //E_LOGIC_OP_BEGIN,
	{ 55, ASSOC_LEFT },		//LOGIC_EQ,			'=='
	{ 55, ASSOC_LEFT },		//LOGIC_NEQ,		'!=' or 'not' keyword
	{ 60, ASSOC_LEFT },		//LOGIC_LT,			'<'
	{ 60, ASSOC_LEFT },		//LOGIC_LET,		'<='
	{ 60, ASSOC_LEFT },		//LOGIC_GT,			'>'
	{ 60, ASSOC_LEFT },		//LOGIC_GET,		'>='
	{ 80, ASSOC_RIGHT },	//LOGIC_NOT,		'!'
	{ 30, ASSOC_LEFT },		//LOGIC_AND,		'&&' or 'and' keyword
	{ 25, ASSOC_LEFT },		//LOGIC_OR,			'||' or 'or' keyword
	s_OpDefault, //E_LOGIC_OP_END,

	s_OpDefault, //E_BITWISE_OP_BEGIN,
	{ 50, ASSOC_LEFT },		//BITWISE_AND,		'&'
	{ 35, ASSOC_LEFT },		//BITWISE_OR,		'|'
	{ 40, ASSOC_LEFT },		//BITWISE_XOR,		'^'
	{ 80, ASSOC_RIGHT },	//BITWISE_NOT,		'~'
	{ 65, ASSOC_LEFT },		//BITWISE_LSH,		'<<'
	{ 65, ASSOC_LEFT },		//BITWISE_RSH,		'>>'
	s_OpDefault, //E_BITWISE_OP_END,

	{ 80, ASSOC_RIGHT },	//UNARY_PLUS,		'+' before an expression (is optional: +5 <=> 5)
	{ 80, ASSOC_RIGHT },	//UNARY_MINUS,		'-' before an expression (-5 is not parsed as 0-5)

	s_OpDefault, //E_MATH_OP_BEGIN,
	{ 70, ASSOC_LEFT },		//MATH_ADD,			'+'
	{ 70, ASSOC_LEFT },		//MATH_SUB,			'-'
	{ 75, ASSOC_LEFT },		//MATH_MUL,			'*'
	{ 75, ASSOC_LEFT },		//MATH_DIV,			'/'
	{ 75, ASSOC_LEFT },		//MATH_MOD,			'%'
	{ 80, ASSOC_RIGHT },	//MATH_INC,			'++'
	{ 80, ASSOC_RIGHT },	//MATH_DEC,			'--'
	s_OpDefault, //E_MATH_OP_END,

	s_OpDefault, //E_ASN_OP_BEGIN,

	{ 10, ASSOC_RIGHT },	//MATH_ASN,			'='
	{ 10, ASSOC_RIGHT },	//MATH_ASN_ADD,		'+='
	{ 10, ASSOC_RIGHT },	//MATH_ASN_SUB,		'-='
	{ 10, ASSOC_RIGHT },	//MATH_ASN_MUL,		'/='
	{ 10, ASSOC_RIGHT },	//MATH_ASN_DIV,		'*='
	{ 10, ASSOC_RIGHT },	//MATH_ASN_MOD,		'%='
	{ 10, ASSOC_RIGHT },	//MATH_ASN_AND,		'&='
	{ 10, ASSOC_RIGHT },	//MATH_ASN_OR,		'|='
	{ 10, ASSOC_RIGHT },	//MATH_ASN_XOR,		'^='
	{ 10, ASSOC_RIGHT },	//MATH_ASN_NOT,		'~='

	s_OpDefault, //E_ASN_OP_END,
};
