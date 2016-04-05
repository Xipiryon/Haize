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
		tokenError(impl, token, "Unexpected '" + token.valueToStr() + "' here!");
	}

	// Whole program
	bool parseChunk(InternalSyntaxicData*);
	// Declarations
	bool parseGlobalDecl(InternalSyntaxicData*);
	bool parseNamespaceDecl(InternalSyntaxicData*);
	bool parseFunctionDecl(InternalSyntaxicData*);
	bool parseArgsDecl(InternalSyntaxicData*);
	bool parseClassDecl(InternalSyntaxicData*);
	bool parseClassCstrDstr(InternalSyntaxicData*);
	bool parseClassMemberDecl(InternalSyntaxicData*);
	// Statements
	bool parseStatements(InternalSyntaxicData*);
	// Control flow, Conditional and Return
	bool parseIf(InternalSyntaxicData*);
	bool parseFor(InternalSyntaxicData*);
	bool parseWhile(InternalSyntaxicData*);
	bool parseSwitch(InternalSyntaxicData*);
	bool parseReturn(InternalSyntaxicData*);
	// Expression, which will use a variant of the Shunting Yard algorithm
	bool parseExprArgsCall(InternalSyntaxicData*, hz::parser::eTokenType);
	bool parseExprNewVarDecl(InternalSyntaxicData*);
	bool parseExprDeleteVarDecl(InternalSyntaxicData*);
	bool parseExprLValue(InternalSyntaxicData*);
	bool parseExpr(InternalSyntaxicData*, hz::parser::eTokenType);
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
			else if (token.type == hz::parser::S_SEPARATOR)
			{
				popToken(impl);
				return true;
			}
		}

		tokenError(impl, impl->lastReadToken);
		return false;
	}

	// ********************************************
	// DECLARATIONS
	// ********************************************

	bool parseGlobalDecl(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;
		popToken(impl);

		m::String typeName;
		m::String varName;

		if (readToken(impl, token) && token.type == hz::parser::V_IDENTIFIER)
		{
			typeName = token.value.get<m::String>();
			popToken(impl);
			if (readToken(impl, token) && token.type == hz::parser::V_IDENTIFIER)
			{
				varName = token.value.get<m::String>();
				popToken(impl);
				if (readToken(impl, token) && token.type == hz::parser::S_SEPARATOR)
				{
					popToken(impl);
					auto* varNode = MUON_NEW(hz::parser::ASTNodeVarDecl);
					varNode->type = hz::parser::NT_GLOBAL_DECL;
					varNode->declTypename = typeName;
					varNode->name = varName;
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

		return false;
	}

	bool parseFunctionDecl(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;
		readToken(impl, token);
		popToken(impl);
		auto* functionNode = MUON_NEW(hz::parser::ASTNodeFunctionDecl);
		functionNode->retTypename = token.value.get<m::String>();
		functionNode->type = hz::parser::NT_FUNCTION_DECL;
		impl->currNode->addChild(functionNode);

		if (readToken(impl, token) && token.type == hz::parser::V_IDENTIFIER)
		{
			popToken(impl);
			functionNode->name = token.value.get<m::String>();
			// Parse arguments
			if (readToken(impl, token) && token.type == hz::parser::S_LPARENT)
			{
				popToken(impl);
				auto* argsNode = MUON_NEW(hz::parser::ASTNode);
				argsNode->type = hz::parser::NT_FUNCTION_ARGS_DECL;
				functionNode->addChild(argsNode);
				// Quick check for a non argument function
				if (readToken(impl, token) && token.type == hz::parser::S_RPARENT)
				{
					popToken(impl);
				}
				else
				{
					// Parse arguments
					impl->currNode = argsNode;
					if (parseArgsDecl(impl))
					{
						if (readToken(impl, token) && token.type == hz::parser::S_RPARENT)
						{
							popToken(impl);
							impl->currNode = functionNode;
						}
						else return false;
					}
					else return false;
				}
			}
			// Parse body
			if (readToken(impl, token) && token.type == hz::parser::S_LBRACE)
			{
				popToken(impl);
				auto* bodyNode = MUON_NEW(hz::parser::ASTNode);
				bodyNode->type = hz::parser::NT_FUNCTION_BODY;
				functionNode->addChild(bodyNode);
				// Same, quick check for an empty body
				if (readToken(impl, token) && token.type == hz::parser::S_RBRACE)
				{
					popToken(impl);
					impl->currNode = functionNode->parent;
					return true;
				}
				else
				{
					impl->currNode = bodyNode;
					if (parseStatements(impl))
					{
						if (readToken(impl, token) && token.type == hz::parser::S_RBRACE)
						{
							popToken(impl);
							impl->currNode = functionNode->parent;
							return true;
						}
					}
				}
			}
		}

		return false;
	}

	bool parseArgsDecl(InternalSyntaxicData* impl)
	{
	label_args_decl_start:
		hz::parser::Token token;
		if (readToken(impl, token))
		{
			hz::parser::ParamPrefix prefix = hz::parser::IN;
			// Prefix is optional
			if (token.type == hz::parser::S_KEYWORD)
			{
				popToken(impl);
				m::String keyword = token.value.get<m::String>();
				if (keyword == "in")
				{
					prefix = hz::parser::IN;
				}
				else if (keyword == "out")
				{
					prefix = hz::parser::OUT;
				}
				else if (keyword == "inout")
				{
					prefix = (hz::parser::ParamPrefix)(hz::parser::IN | hz::parser::OUT);
				}
				else
				{
					tokenError(impl, token);
					return false;
				}
				// Read next Token, should be Identifier (decl type)
				readToken(impl, token);
			}

			if (token.type == hz::parser::V_IDENTIFIER)
			{
				popToken(impl);
				m::String declTypename = token.value.get<m::String>();
				readToken(impl, token);
				if (token.type == hz::parser::V_IDENTIFIER)
				{
					popToken(impl);
					auto* argNode = MUON_NEW(hz::parser::ASTNodeArgDecl);
					impl->currNode->addChild(argNode);
					argNode->prefix = prefix;
					argNode->declTypename = declTypename;
					argNode->name = token.value.get<m::String>();
					if (readToken(impl, token))
					{
						// Now look for a closing parenthesis, which mean the end of args list
						// (dont' pop it, that's the role of the parseFuncDecl)
						if (token.type == hz::parser::S_RPARENT)
						{
							return true;
						}
						// Or, we could have a comma, and then parse again
						else if (token.type == hz::parser::S_COMMA)
						{
							popToken(impl);
							goto label_args_decl_start;
						}
					}
				}
			}
		}

		tokenError(impl, token);
		return false;
	}

	bool parseClassDecl(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;
		popToken(impl);

		if (readToken(impl, token) && token.type == hz::parser::V_IDENTIFIER)
		{
			popToken(impl);
			auto* classNode = MUON_NEW(hz::parser::ASTNode);
			impl->currNode->addChild(classNode);
			impl->currNode = classNode;
			classNode->type = hz::parser::NT_CLASS_DECL;
			classNode->name = token.value.get<m::String>();
			if (readToken(impl, token) && token.type == hz::parser::S_LBRACE)
			{
				popToken(impl);
				// Quick check for an empty body
				if (readToken(impl, token) && token.type == hz::parser::S_RBRACE)
				{
					popToken(impl);
					impl->currNode = classNode->parent;
					return true;
				}
				else
				{
				label_class_body_start:

					// Purge lone ';'
					if (readToken(impl, token) && token.type == hz::parser::S_SEPARATOR)
					{
						popToken(impl);
						goto label_class_body_start;
					}

					// Read two identifier, the decltype and the name.
					// The 3rd token determine if either a function ('(') or a member (';')
					// Special case for 'constructor' and 'destructor'
					hz::parser::Token declType;
					hz::parser::Token declName;
					if (readToken(impl, declType, 1, true)
						&& readToken(impl, declName, 2, true)
						&& readToken(impl, token, 3, true))
					{
						if (declType.type == hz::parser::S_KEYWORD)
						{
							m::String keyword = declType.value.get<m::String>();
							if (keyword == "constructor" || keyword == "destructor")
							{
								if (!parseClassCstrDstr(impl))
								{
									return false;
								}
							}
							else
							{
								tokenError(impl, declType);
							}
						}
						else if (token.type == hz::parser::S_LPARENT)
						{
							if (!parseFunctionDecl(impl))
							{
								return false;
							}
						}
						else if (token.type == hz::parser::S_SEPARATOR)
						{
							if (!parseClassMemberDecl(impl))
							{
								return false;
							}
						}
						else
						{
							tokenError(impl, token);
						}

						// Member / Function has been parsed, check for class end, or reloop
						if (readToken(impl, token) && token.type == hz::parser::S_RBRACE)
						{
							popToken(impl);
							impl->currNode = classNode->parent;
							return true;
						}
						else goto label_class_body_start;
					}
				}
			}
		}

		return false;
	}

	bool parseClassMemberDecl(InternalSyntaxicData* impl)
	{
		// Check has been made in parse class function, so just unwrap and make a node
		hz::parser::Token token;
		auto* argDecl = MUON_NEW(hz::parser::ASTNodeVarDecl);
		argDecl->type = hz::parser::NT_CLASS_MEMBER;

		readToken(impl, token);
		popToken(impl);
		argDecl->declTypename = token.value.get<m::String>();

		readToken(impl, token);
		popToken(impl);
		argDecl->name = token.value.get<m::String>();

		popToken(impl); // ';'

		return true;
	}

	bool parseClassCstrDstr(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;
		readToken(impl, token);
		popToken(impl);
		m::String keyword = token.value.get<m::String>();
		auto* cdstrNode = MUON_NEW(hz::parser::ASTNodeFunctionDecl);
		cdstrNode->type = (keyword == "constructor"
						   ? hz::parser::NT_CLASS_CONSTRUCTOR
						   : hz::parser::NT_CLASS_DESTRUCTOR);

		cdstrNode->name = keyword;
		impl->currNode->addChild(cdstrNode);

		// Parse arguments
		if (readToken(impl, token) && token.type == hz::parser::S_LPARENT)
		{
			popToken(impl);
			auto* argsNode = MUON_NEW(hz::parser::ASTNode);
			argsNode->type = hz::parser::NT_FUNCTION_ARGS_DECL;
			cdstrNode->addChild(argsNode);
			// Quick check for a non argument function
			if (readToken(impl, token) && token.type == hz::parser::S_RPARENT)
			{
				popToken(impl);
			}
			else
			{
				// Parse arguments
				impl->currNode = argsNode;
				if (parseArgsDecl(impl))
				{
					if (readToken(impl, token) && token.type == hz::parser::S_RPARENT)
					{
						popToken(impl);
						impl->currNode = cdstrNode;
					}
					else return false;
				}
				else return false;
			}
		}
		// Parse body
		if (readToken(impl, token) && token.type == hz::parser::S_LBRACE)
		{
			popToken(impl);
			auto* bodyNode = MUON_NEW(hz::parser::ASTNode);
			bodyNode->type = hz::parser::NT_FUNCTION_BODY;
			cdstrNode->addChild(bodyNode);
			// Same, quick check for an empty body
			if (readToken(impl, token) && token.type == hz::parser::S_RBRACE)
			{
				popToken(impl);
				impl->currNode = cdstrNode->parent;
				return true;
			}
			else
			{
				impl->currNode = bodyNode;
				if (parseStatements(impl))
				{
					if (readToken(impl, token) && token.type == hz::parser::S_RBRACE)
					{
						popToken(impl);
						impl->currNode = cdstrNode->parent;
						return true;
					}
				}
			}
		}

		return false;
	}

	// ********************************************
	// STATEMENT AND CONTROL FLOW
	// ********************************************

	bool parseStatements(InternalSyntaxicData* impl)
	{
	label_statement_start:
		hz::parser::Token token;
		readToken(impl, token);
		if (token.type == hz::parser::S_KEYWORD)
		{
			m::String keyword = token.value.get<m::String>();
			if (keyword == "new")
			{
				if (parseExprNewVarDecl(impl))
				{
					goto label_statement_check_rec;
				}
			}
			else if (keyword == "delete")
			{
				if (parseExprDeleteVarDecl(impl))
				{
					goto label_statement_check_rec;
				}
			}
			else if (keyword == "return")
			{
				if (parseReturn(impl))
				{
					goto label_statement_check_rec;
				}
			}
			else if (keyword == "if")
			{
				if (parseIf(impl))
				{
					goto label_statement_check_rec;
				}
			}
			else if (keyword == "for")
			{
				if (parseFor(impl))
				{
					goto label_statement_check_rec;
				}
			}
			else if (keyword == "while")
			{
				if (parseWhile(impl))
				{
					goto label_statement_check_rec;
				}
			}
			else if (keyword == "switch")
			{
				if (parseSwitch(impl))
				{
					goto label_statement_check_rec;
				}
			}
			else
			{
				tokenError(impl, token);
			}
		}
		else if (token.type == hz::parser::V_IDENTIFIER)
		{
			auto* asnNode = MUON_NEW(hz::parser::ASTNode);
			asnNode->type = hz::parser::E_ASN_OP_BEGIN;
			impl->currNode->addChild(asnNode);
			impl->currNode = asnNode;
			if (parseExprLValue(impl))
			{
				if (readToken(impl, token)
					&& (token.type > hz::parser::E_ASN_OP_BEGIN && token.type < hz::parser::E_ASN_OP_END))
				{
					popToken(impl);
					asnNode->type = token.type; // Set the real type
					if (parseExpr(impl, hz::parser::S_SEPARATOR))
					{
						goto label_statement_check_rec;
					}
				}
			}
		}
		// Purge lone ';'
		else if (token.type == hz::parser::S_SEPARATOR)
		{
			popToken(impl);
			goto label_statement_start;
		}
		else
		{
			tokenError(impl, token);
		}
		return false;
		// Do a global check here: we should have a closing brace, else restart
		// (do not pop the brace, the constructor/destructor/function body will do)
	label_statement_check_rec:
		readToken(impl, token);
		if (token.type != hz::parser::S_LBRACE)
		{
			goto label_statement_start;
		}
		return true;
	}

	// Control flow, Conditional and Return
	bool parseIf(InternalSyntaxicData*)
	{
		hz::parser::Token token;

		return false;
	}

	bool parseFor(InternalSyntaxicData*)
	{
		hz::parser::Token token;

		return false;
	}

	bool parseWhile(InternalSyntaxicData*)
	{
		hz::parser::Token token;

		return false;
	}

	bool parseSwitch(InternalSyntaxicData*)
	{
		hz::parser::Token token;

		return false;
	}
	bool parseReturn(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;

		return false;
	}

	// ********************************************
	// EXPRESSION
	// ********************************************

	bool parseExprArgsCall(InternalSyntaxicData* impl, hz::parser::eTokenType endTokenType)
	{

		return false;
	}

	bool parseExprNewVarDecl(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;

		return false;
	}

	bool parseExprDeleteVarDecl(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;

		return false;
	}

	bool parseExprLValue(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;

		return false;
	}

	bool parseExpr(InternalSyntaxicData* impl, hz::parser::eTokenType endTokenType)
	{
		hz::parser::Token token;

		return false;
	}
}
