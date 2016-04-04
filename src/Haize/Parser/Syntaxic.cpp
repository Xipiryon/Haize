#include <Muon/System/Log.hpp>

#include "Haize/Parser/Compiler.hpp"

namespace
{
	m::String tokenValueToStr(const hz::parser::Token& token)
	{
		char buffer[32];
		m::String tokstr;
		m::meta::MetaData* m = token.value.getMeta();
		if (MUON_META(m::String) == m)
		{
			tokstr = token.value.get<m::String>();
		}
		else if (MUON_META(m::f32) == m)
		{
			m::ftoa(token.value.get<m::f32>(), buffer);
			tokstr = buffer;
		}
		else if (MUON_META(m::i32) == m)
		{
			m::itoa((m::i64)token.value.get<m::i32>(), buffer);
			tokstr = buffer;
		}
		else
		{
			tokstr = hz::parser::TokenTypeStr[token.type];
		}
		return tokstr;
	}

	// ASCII Tree variables
	char depth[2048];
	m::u32 depthIndex;

	void push(char c)
	{
		depth[depthIndex++] = ' ';
		depth[depthIndex++] = c;
		depth[depthIndex++] = ' ';
		depth[depthIndex++] = ' ';
		depth[depthIndex] = 0;
	}

	void pop()
	{
		depth[depthIndex -= 4] = 0;
	}

	void display(hz::parser::ASTNode* node)
	{
#if defined(HAIZE_DEBUG)
		printf(" %s (%s)\n", node->name.cStr(), tokenValueToStr(node->token).cStr());
		for (m::u32 i = 0; i < node->children->size(); ++i)
		{
			hz::parser::ASTNode* n = (*node->children)[i];
			if (i < node->children->size() - 1)
			{
#if defined(MUON_PLATFORM_WINDOWS)
				printf(" %s |-", depth);
#else
				printf("%s ├─", depth);
#endif
				push('|');
			}
			else // Last element
			{
#if defined(MUON_PLATFORM_WINDOWS)
				printf(" %s '-", depth);
#else
				printf(" %s └─", depth);
#endif
				push(' ');
			}
			display(n);
			pop();
#endif
		}
	}

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

	/*
	* Custom SharedData
	* Specific for Lexical parsing
	*/
	struct InternalSyntaxicData : public hz::parser::InternalCompilerData
	{
		InternalSyntaxicData(hz::Error& err)
			: error(err)
		{
		}

		virtual ~InternalSyntaxicData()
		{
		}

		std::vector<hz::parser::Token>* tokenList;
		hz::parser::ASTNode* rootNode;

		hz::Error& error;
		m::u32 readTokenIndex;

		std::deque<hz::parser::Token> exprTokens;
		std::deque<hz::parser::ASTNode*> exprNodes;
		hz::parser::ASTNode* phaseNode;
		bool funcCstrDstr;
	};

	static const OpAttribute precAttrib[hz::parser::E_TERMINALTOKEN_END] =
	{
		s_OpDefault, //S_INVALID,
		s_OpDefault, //S_EOF,

		s_OpDefault, //S_COMMA,			','
		{ 85, ASSOC_RIGHT },	//S_ACCESSOR,		'.'
		s_OpDefault, //S_SEPARATOR,		New line, or ';'
		s_OpDefault, //S_KEYWORD,
		s_OpDefault, //S_LPARENT,		'('
		s_OpDefault, //S_RPARENT,		')'
		s_OpDefault, //S_LBRACKET,		'['
		s_OpDefault, //S_RBRACKET,		']'
		s_OpDefault, //S_LBRACE,		'{'
		s_OpDefault, //S_RBRACE,		'}'

		s_OpDefault, //E_VALUE_BEGIN,
		s_OpDefault, //V_NUMBER,			A number (in either dec, oct, bin or hex format)
		s_OpDefault, //V_IDENTIFIER,		An alphanumeric identifier
		s_OpDefault, //V_STRING,			A string contained between two '"' character
		s_OpDefault, //V_NIL,
		s_OpDefault, //V_TRUE,
		s_OpDefault, //V_FALSE,
		s_OpDefault, //E_VALUE_END,

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

	bool readToken(InternalSyntaxicData* impl, hz::parser::Token& out, m::u32 index)
	{
		index += impl->readTokenIndex;
		if (index < impl->tokenList->size())
		{
			out = impl->tokenList->at(index);
			return true;
		}
		return false;
	}

	bool popToken(InternalSyntaxicData* impl, m::u32 count)
	{
		if ((impl->readTokenIndex + count) <= impl->tokenList->size())
		{
			impl->readTokenIndex += count;
			return true;
		}
		return false;
	}

	void tokenError(hz::Error& error, const hz::parser::Token& token, const m::String& msg)
	{
		error.state = hz::Error::ERROR;
		error.section = token.section;
		error.line = token.line;
		error.column = token.column;
		error.message = msg;
	}

	void tokenError(hz::Error& error, const hz::parser::Token& token)
	{
		tokenError(error, token, tokenValueToStr(token));
	}

	// Whole program
	bool parseChunk(InternalSyntaxicData*);
	// Declarations
	bool parseGlobalDecl(InternalSyntaxicData*);
	bool parseNamespaceDecl(InternalSyntaxicData*);
	bool parseFunctionDecl(InternalSyntaxicData*);
	bool parseArgsDecl(InternalSyntaxicData*);
	bool parseClassDecl(InternalSyntaxicData*);
	bool parseClassMemberDecl(InternalSyntaxicData*);
	// Statement and control flow
	bool parseStatement(InternalSyntaxicData*);
	bool parseControlFlow(InternalSyntaxicData*);
	bool parseConditionalFlow(InternalSyntaxicData*);
	// Expression, which will use a variant of the Shunting Yard algorithm
	bool parseNewVarDecl(InternalSyntaxicData*);
	bool parseDeleteVarDecl(InternalSyntaxicData*);
	bool parseAsnExpr(InternalSyntaxicData*);
	bool parseReturnExpr(InternalSyntaxicData*);
	bool parseExpr(InternalSyntaxicData*);
}

namespace hz
{
	namespace parser
	{
		bool Compiler::syntaxic(Error& error)
		{
			error.clear();
			error.step = Error::COMPILATION;

			// Clean children
			while (!m_nodeRoot->children->empty())
			{
				MUON_DELETE(m_nodeRoot->children->back());
				m_nodeRoot->children->pop_back();
			}

			// Reinit token
			m_nodeRoot->name = "#ROOT#";
			m_nodeRoot->token.type = parser::NT_ROOT;

			InternalSyntaxicData impl(error);
			impl.readTokenIndex = 0;
			impl.phaseNode = m_nodeRoot;
			impl.funcCstrDstr = false;

			// Skip empty Token list
			if (m_tokenList->empty())
			{
				error.state = Error::SUCCESS;
				return true;
			}

			parser::Token token;
			while (impl.readTokenIndex < m_tokenList->size())
			{
				if (!parseChunk(&impl))
				{
					error.state = Error::ERROR;
					return false;
				}
			}

			error.state = Error::SUCCESS;
#if defined(HAIZE_DEBUG)
			m::system::Log debug("Syntaxic", m::LOG_DEBUG);
			display(m_nodeRoot);
#endif
			return true;
		}
	}
}

namespace
{
	// Whole program
	bool parseChunk(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;
		bool ok = true;
		return true;
	}

	// Declarations
	bool parseGlobalDecl(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;
		bool ok = true;
		return true;
	}

	bool parseNamespaceDecl(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;
		bool ok = true;
		return true;
	}

	bool parseFunctionDecl(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;
		bool ok = true;
		return true;
	}

	bool parseArgsDecl(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;
		bool ok = true;
		return true;
	}

	bool parseClassDecl(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;
		bool ok = true;
		return true;
	}

	bool parseClassMemberDecl(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;
		bool ok = true;
		return true;
	}

	// Statement and control flow
	bool parseStatement(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;
		bool ok = true;
		return true;
	}

	bool parseControlFlow(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;
		bool ok = true;
		return true;
	}

	bool parseConditionalFlow(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;
		bool ok = true;
		return true;
	}

	// Expression, which will use a variant of the Shunting Yard algorithm
	bool parseNewVarDecl(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;
		bool ok = true;
		return true;
	}

	bool parseDeleteVarDecl(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;
		bool ok = true;
		return true;
	}

	bool parseAsnExpr(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;
		bool ok = true;
		return true;
	}

	bool parseReturnExpr(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;
		bool ok = true;
		return true;
	}

	bool parseExpr(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;
		bool ok = true;
		return true;
	}
}
