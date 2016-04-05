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
			hz::parser::ASTNode* n = node->children->at(i);
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

		std::vector<hz::parser::ASTNode*> exprValue;
		std::vector<hz::parser::ASTNode*> exprOperator;
		hz::parser::ASTNode* currNode;
	};

	enum OpAssociativity
	{
		ASSOC_LEFT,
		ASSOC_RIGHT,
	};

	struct OpInfo
	{
		OpInfo()
			: precedence(0)
			, associativity(ASSOC_LEFT)
		{
		}

		OpInfo(m::u32 prec, OpAssociativity assoc)
			: precedence(prec)
			, associativity(assoc)
		{
		}

		m::u32 precedence;
		OpAssociativity associativity;
	};

	struct OpAttributeTable
	{
		OpAttributeTable()
		{
			table[hz::parser::MATH_ASN] = OpInfo(10, ASSOC_RIGHT);		//'='
			table[hz::parser::MATH_ASN_ADD] = OpInfo(10, ASSOC_RIGHT);	//'+='
			table[hz::parser::MATH_ASN_SUB] = OpInfo(10, ASSOC_RIGHT);	//'-='
			table[hz::parser::MATH_ASN_MUL] = OpInfo(10, ASSOC_RIGHT);	//'/='
			table[hz::parser::MATH_ASN_DIV] = OpInfo(10, ASSOC_RIGHT);	//'*='
			table[hz::parser::MATH_ASN_MOD] = OpInfo(10, ASSOC_RIGHT);	//'%='
			table[hz::parser::MATH_ASN_AND] = OpInfo(10, ASSOC_RIGHT);	//'&='
			table[hz::parser::MATH_ASN_OR] = OpInfo(10, ASSOC_RIGHT);	//'|='
			table[hz::parser::MATH_ASN_XOR] = OpInfo(10, ASSOC_RIGHT);	//'^='
			table[hz::parser::MATH_ASN_NOT] = OpInfo(10, ASSOC_RIGHT);	//'~='

			table[hz::parser::LOGIC_OR] = OpInfo(25, ASSOC_LEFT);		//'||'

			table[hz::parser::LOGIC_AND] = OpInfo(30, ASSOC_LEFT);		//'&&'

			table[hz::parser::BITWISE_OR] = OpInfo(35, ASSOC_LEFT);		//'|'

			table[hz::parser::BITWISE_XOR] = OpInfo(40, ASSOC_LEFT);	//'^'

			table[hz::parser::BITWISE_AND] = OpInfo(50, ASSOC_LEFT);	//'&'

			table[hz::parser::LOGIC_EQ] = OpInfo(55, ASSOC_LEFT);		//'=='
			table[hz::parser::LOGIC_NEQ] = OpInfo(55, ASSOC_LEFT);		//'!='

			table[hz::parser::LOGIC_LT] = OpInfo(60, ASSOC_LEFT);		//'<'
			table[hz::parser::LOGIC_LET] = OpInfo(60, ASSOC_LEFT);		//'<='
			table[hz::parser::LOGIC_GT] = OpInfo(60, ASSOC_LEFT);		//'>'
			table[hz::parser::LOGIC_GET] = OpInfo(60, ASSOC_LEFT);		//'>='

			table[hz::parser::BITWISE_LSH] = OpInfo(65, ASSOC_LEFT);	//'<<'
			table[hz::parser::BITWISE_RSH] = OpInfo(65, ASSOC_LEFT);	//'>>'

			table[hz::parser::MATH_ADD] = OpInfo(70, ASSOC_LEFT);		//'+'
			table[hz::parser::MATH_SUB] = OpInfo(70, ASSOC_LEFT);		//'-'

			table[hz::parser::MATH_MUL] = OpInfo(75, ASSOC_LEFT);		//'*'
			table[hz::parser::MATH_DIV] = OpInfo(75, ASSOC_LEFT);		//'/'
			table[hz::parser::MATH_MOD] = OpInfo(75, ASSOC_LEFT);		//'%'

			table[hz::parser::UNARY_PLUS] = OpInfo(80, ASSOC_RIGHT);	//'+' before an expression (is optional: +5 <=> 5)
			table[hz::parser::UNARY_MINUS] = OpInfo(80, ASSOC_RIGHT);	//'-' before an expression (-5 is not parsed as 0-5)
			table[hz::parser::BITWISE_NOT] = OpInfo(80, ASSOC_RIGHT);	//'~'
			table[hz::parser::LOGIC_NOT] = OpInfo(80, ASSOC_RIGHT);		//'!'
			table[hz::parser::MATH_INC] = OpInfo(80, ASSOC_RIGHT);		//'++'
			table[hz::parser::MATH_DEC] = OpInfo(80, ASSOC_RIGHT);		//'--'

			table[hz::parser::S_ACCESSOR] = OpInfo(85, ASSOC_RIGHT);	//'.'
		}

		OpInfo operator[](hz::parser::eTokenType type) const
		{
			return table[type];
		}

		OpInfo table[hz::parser::E_TERMINALTOKEN_END];
	};
	const OpAttributeTable s_PrecedenceTable;

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
	bool canMergeOperatorValue(InternalSyntaxicData*);
	bool mergeOperatorValueAll(InternalSyntaxicData*);
	bool mergeOperatorValue(InternalSyntaxicData*);
	bool parseExprArgsCall(InternalSyntaxicData*, hz::parser::eTokenType);
	bool parseExprNewVarDecl(InternalSyntaxicData*);
	bool parseExprDeleteVarDecl(InternalSyntaxicData*);
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
					for (auto it : impl.exprValue)
					{
						MUON_DELETE(it);
					}
					for (auto it : impl.exprOperator)
					{
						MUON_DELETE(it);
					}
					return false;
				}
			}

			error.state = Error::SUCCESS;
#if defined(HAIZE_DEBUG)
			m::system::Log debug("Syntaxic", m::LOG_DEBUG);
			display(m_nodeRoot);
#endif
			for (auto it : impl.exprValue)
			{
				MUON_DELETE(it);
			}
			for (auto it : impl.exprOperator)
			{
				MUON_DELETE(it);
			}
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
		// Purge lone ';'
		else if (token.type == hz::parser::S_SEPARATOR)
		{
			popToken(impl);
			goto label_statement_start;
		}
		// Check we're at the function end
		else if (impl->currNode->type == hz::parser::NT_FUNCTION_BODY
				 && token.type == hz::parser::S_RBRACE)
		{
			// Token will be poped by parseFunctionDecl, and current node impl too
			return true;
		}
		// An error occured somewhere
		else if (!parseExpr(impl, hz::parser::S_SEPARATOR))
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

	bool canMergeOperatorValue(InternalSyntaxicData* impl)
	{
		return (!impl->exprOperator.empty()
				|| (impl->exprOperator.empty() && impl->exprValue.size() == 1));
	}

	bool mergeOperatorValueAll(InternalSyntaxicData* impl)
	{
		while (canMergeOperatorValue(impl))
		{
			if (!mergeOperatorValue(impl))
			{
				return false;
			}
		}
		return true;
	}

	bool mergeOperatorValue(InternalSyntaxicData* impl)
	{
		if (impl->exprOperator.empty() && (impl->exprValue.size() == 1))
		{
			impl->currNode->addChild(impl->exprValue.back());
			impl->exprValue.pop_back();
			return true;
		}
		else if (!impl->exprOperator.empty())
		{
			auto* op = impl->exprOperator.back();
			// Binary operators
			if (op->type == hz::parser::S_ACCESSOR
				|| (op->type > hz::parser::E_BITWISE_OP_BEGIN && op->type < hz::parser::E_BITWISE_OP_END)
				|| (op->type > hz::parser::E_LOGIC_OP_BEGIN && op->type < hz::parser::E_LOGIC_OP_END)
				|| (op->type > hz::parser::E_MATH_OP_BEGIN && op->type < hz::parser::E_MATH_OP_END))
			{
				if (impl->exprValue.size() >= 2)
				{
					auto* right = impl->exprValue.back();
					impl->exprValue.pop_back();
					auto* left = impl->exprValue.back();
					impl->exprValue.pop_back();
					op->addChild(left);
					op->addChild(right);
					impl->currNode->addChild(op);
					impl->exprOperator.pop_back();
				}
			}
		}

		return false;
	}

	bool parseExprArgsCall(InternalSyntaxicData* impl, hz::parser::eTokenType endTokenType)
	{
		hz::parser::Token token;
		if (!readToken(impl, token))
		{
			return false;
		}

		if (token.type == endTokenType)
		{
			popToken(impl);
			return true;
		}
		return parseExpr(impl, endTokenType);
	}

	bool parseExprNewVarDecl(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;
		readToken(impl, token);
		popToken(impl);

		auto* newNode = MUON_NEW(hz::parser::ASTNodeVarDecl);
		newNode->global = false;
		newNode->type = hz::parser::NT_EXPR_NEW;
		impl->currNode->addChild(newNode);
		impl->currNode = newNode;

		if (readToken(impl, token) && token.type == hz::parser::V_IDENTIFIER)
		{
			popToken(impl);
			m::String declTypename = token.value.get<m::String>();
			if (readToken(impl, token) && token.type == hz::parser::V_IDENTIFIER)
			{
				popToken(impl);
				newNode->declTypename = declTypename;
				newNode->name = token.value.get<m::String>();
				// Ok now, let's search for ';' (no value), '=' or '(' for constructor with value
				if (readToken(impl, token))
				{
					if (token.type == hz::parser::S_SEPARATOR)
					{
						popToken(impl);
						impl->currNode = impl->currNode->parent;
						return true;
					}
					else if (token.type == hz::parser::MATH_ASN)
					{
						if (parseExprArgsCall(impl, hz::parser::S_SEPARATOR))
						{
							impl->currNode = impl->currNode->parent;
							return true;
						}
					}
					else if (token.type == hz::parser::S_LPARENT)
					{
						popToken(impl);
						if (parseExprArgsCall(impl, hz::parser::S_RPARENT))
						{
							if (readToken(impl, token) && token.type == hz::parser::S_SEPARATOR)
							{
								popToken(impl);
								impl->currNode = impl->currNode->parent;
								return true;
							}
						}
						tokenError(impl, token);
					}
				}
			}
			tokenError(impl, token);
		}
		tokenError(impl, token);

		return false;
	}

	bool parseExprDeleteVarDecl(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;
		readToken(impl, token);
		popToken(impl);

		auto* delNode = MUON_NEW(hz::parser::ASTNode);
		delNode->type = hz::parser::NT_EXPR_DELETE;
		impl->currNode->addChild(delNode);

		if (readToken(impl, token))
		{
			popToken(impl);
			if (token.type == hz::parser::V_IDENTIFIER)
			{
				delNode->name = token.value.get<m::String>();
				if (readToken(impl, token) && token.type == hz::parser::S_SEPARATOR)
				{
					popToken(impl);
					return true;
				}
			}
			else if (token.type == hz::parser::S_LPARENT)
			{
				if (readToken(impl, token) && token.type == hz::parser::V_IDENTIFIER)
				{
					popToken(impl);
					delNode->name = token.value.get<m::String>();
					if (readToken(impl, token) && token.type == hz::parser::S_RPARENT)
					{
						popToken(impl);
						if (readToken(impl, token) && token.type == hz::parser::S_SEPARATOR)
						{
							popToken(impl);
							return true;
						}
					}
				}
			}
		}
		return false;
	}

	bool parseExpr(InternalSyntaxicData* impl, hz::parser::eTokenType endTokenType)
	{
		hz::parser::Token token;
		bool ok = readToken(impl, token);
		bool parse = true;
		while (ok && parse)
		{
			parse = token.type != endTokenType;
			if (!parse)
			{
				popToken(impl);
				bool err = false;
				// Merge as long as we can
				if (!mergeOperatorValueAll(impl))
				{
					tokenError(impl, token);
					return false;
				}
				return true;
			}

			// Expr, Expr is an allowed syntax, where the latest Expr value will be returned
			if (token.type == hz::parser::S_COMMA)
			{
				if (!mergeOperatorValueAll(impl))
				{
					tokenError(impl, token);
					return false;
				}
			}
			// Constant
			else if (token.type > hz::parser::E_CONSTANT_BEGIN && token.type < hz::parser::E_CONSTANT_END)
			{
				auto* constNode = MUON_NEW(hz::parser::ASTNodeConstant);
				constNode->type = token.type;
				constNode->value = token.value;
				impl->exprValue.push_back(constNode);
			}
			// Identifier
			else if (token.type == hz::parser::V_IDENTIFIER)
			{
				auto* identNode = MUON_NEW(hz::parser::ASTNode);
				identNode->type = token.type;
				identNode->name = token.value.get<m::String>();
				impl->exprValue.push_back(identNode);
			}
			// Operator
			else
			{
				auto* opNode = MUON_NEW(hz::parser::ASTNode);
				opNode->type = token.type;
				OpInfo currInfo = s_PrecedenceTable[token.type];
				bool reduce = !impl->exprOperator.empty();
				while (reduce)
				{
					OpInfo prevInfo = s_PrecedenceTable[impl->exprOperator.back()->type];
					// We need to reduce if previous operator as a right associativity and current one is left,
					// or if the previous operator as a higher precedence than current.
					// We don't need to merge in other cases
					if ((prevInfo.associativity == ASSOC_RIGHT && currInfo.associativity == ASSOC_LEFT)
						|| (prevInfo.precedence > currInfo.precedence))
					{
						if (!mergeOperatorValue(impl))
						{
							tokenError(impl, token);
							return false;
						}
						reduce = !impl->exprOperator.empty();
					}
					else
					{
						reduce = false;
					}
				}

				impl->exprOperator.push_back(opNode);
			}

			popToken(impl);
			ok = readToken(impl, token);
		}

		return false;
	}
}
