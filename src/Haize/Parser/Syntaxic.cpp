#include <Muon/System/Log.hpp>

#include "Haize/Parser/Compiler.hpp"

namespace
{
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
		printf(" %s \n", node->toString().cStr());
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
		std::vector<hz::parser::Compiler::Section>* sections;
		hz::parser::ASTNode* rootNode;

		hz::Error& error;
		m::u32 readTokenIndex;

		hz::parser::Token lastReadToken;

		std::deque<hz::parser::Token> exprTokens;
		std::deque<hz::parser::ASTNode*> exprNodes;
		hz::parser::ASTNode* currNode;
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

	bool readToken(InternalSyntaxicData* impl, hz::parser::Token& out, m::u32 index, bool fillError)
	{
		index += impl->readTokenIndex - 1;
		if (index < impl->tokenList->size())
		{
			out = impl->tokenList->at(index);
			impl->lastReadToken = out;
			return true;
		}
		// Setup error with last token info, as we have no more token to read
		if (fillError)
		{
			impl->error.line = impl->tokenList->back().line;
			impl->error.column = impl->tokenList->back().column;
			impl->error.section = impl->sections->at(impl->tokenList->back().section).name;
			impl->error.message = "Unexpected EOF";
			impl->error.state = hz::Error::ERROR;
		}
		return false;
	}

	bool readToken(InternalSyntaxicData* impl, hz::parser::Token& out, bool fillError = true)
	{
		return readToken(impl, out, 1, fillError);
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

	bool popToken(InternalSyntaxicData* impl)
	{
		return popToken(impl, 1);
	}

	void tokenError(InternalSyntaxicData* impl, const hz::parser::Token& token, const m::String& msg)
	{
		impl->error.state = hz::Error::ERROR;
		impl->error.section = impl->sections->at(token.section).name;
		impl->error.line = token.line;
		impl->error.column = token.column;
		impl->error.message = msg;
	}

	void tokenError(InternalSyntaxicData* impl, const hz::parser::Token& token)
	{
		tokenError(impl, token, token.valueToStr());
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
			m_nodeRoot->type = parser::NT_ROOT;

			InternalSyntaxicData impl(error);
			impl.tokenList = m_tokenList;
			impl.sections = m_sections;
			impl.rootNode = m_nodeRoot;
			impl.readTokenIndex = 0;
			impl.currNode = m_nodeRoot;
			impl.funcCstrDstr = false;

			// Skip empty Token list
			if (m_tokenList->empty())
			{
				error.state = Error::SUCCESS;
				return true;
			}

			impl.lastReadToken = m_tokenList->front();
			parser::Token token;
			while (readToken(&impl, token))
			{
				if (token.type == S_EOF)
				{
					break;
				}

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
		if (readToken(impl, token))
		{
			if (token.type == hz::parser::S_KEYWORD)
			{
				m::String keyword = token.value.get<m::String>();
				if (keyword == "namespace")
				{
					return parseNamespaceDecl(impl);
				}
				else if (keyword == "class")
				{
					return parseClassDecl(impl);
				}
				else if (keyword == "global")
				{
					return parseGlobalDecl(impl);
				}
			}
			else if (token.type == hz::parser::V_IDENTIFIER)
			{
				return parseFunctionDecl(impl);
			}
		}

		tokenError(impl, impl->lastReadToken);
		return false;
	}

	// Declarations
	bool parseGlobalDecl(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;
		readToken(impl, token); // Global token
		popToken(impl);

		m::String typeName;
		m::String varName;

		if (readToken(impl, token) && token.type == hz::parser::V_IDENTIFIER)
		{
			typeName = token.value.get<m::String>();
			popToken(impl, 1);
			if (readToken(impl, token) && token.type == hz::parser::V_IDENTIFIER)
			{
				varName = token.value.get<m::String>();
				popToken(impl, 1);
				if (readToken(impl, token) && token.type == hz::parser::S_SEPARATOR)
				{
					popToken(impl);
					auto* varNode = MUON_NEW(hz::parser::ASTNodeVarDecl);
					varNode->type = hz::parser::NT_GLOBAL_DECL;
					varNode->typeName = typeName;
					varNode->varName = varName;
					varNode->global = true;
					impl->currNode->addChild(varNode);
					return true;
				}
			}
		}

		return false;
	}

	bool parseNamespaceDecl(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;
		readToken(impl, token);
		popToken(impl);
		hz::parser::ASTNode* namespaceNode = MUON_NEW(hz::parser::ASTNodeNamespaceDecl);
		namespaceNode->type = hz::parser::NT_NAMESPACE_DECL;

		if (readToken(impl, token) && token.type == hz::parser::V_IDENTIFIER)
		{
			popToken(impl);
			namespaceNode->name = token.value.get<m::String>();
			if (readToken(impl, token) && token.type == hz::parser::S_LBRACE)
			{
				popToken(impl);
				// Check for a closing brace: will mean the namespace is empty
				if (readToken(impl, token) && token.type == hz::parser::S_RBRACE)
				{
					popToken(impl);
					// Don't even add it, just delete it and return
					MUON_DELETE(namespaceNode);
					return true;
				}

				// Continue parsing, and check for a closing brace
				impl->currNode->addChild(namespaceNode);
				impl->currNode = namespaceNode;
				if (parseChunk(impl))
				{
					if (readToken(impl, token) && token.type == hz::parser::S_RBRACE)
					{
						popToken(impl);
						impl->currNode = namespaceNode->parent;
						return true;
					}
				}
			}
		}

		MUON_DELETE(namespaceNode);
		return false;
	}

	bool parseFunctionDecl(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;

		return false;
	}

	bool parseArgsDecl(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;

		return false;
	}

	bool parseClassDecl(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;

		return false;
	}

	bool parseClassMemberDecl(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;

		return false;
	}

	// Statement and control flow
	bool parseStatement(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;

		return false;
	}

	bool parseControlFlow(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;

		return false;
	}

	bool parseConditionalFlow(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;

		return false;
	}

	// Expression, which will use a variant of the Shunting Yard algorithm
	bool parseNewVarDecl(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;

		return false;
	}

	bool parseDeleteVarDecl(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;

		return false;
	}

	bool parseAsnExpr(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;

		return false;
	}

	bool parseReturnExpr(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;

		return false;
	}

	bool parseExpr(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;

		return false;
	}
}
