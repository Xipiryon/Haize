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

	void displayNodeHierarchy(hz::parser::ASTNode* node)
	{
#if defined(HAIZE_DEBUG)
		printf(" %s: %s \n"
			   , hz::parser::TokenTypeStr[node->type]
			   , node->toString().cStr());
		for (m::u32 i = 0; i < node->children->size(); ++i)
		{
			hz::parser::ASTNode* n = node->children->at(i);
			if (i < node->children->size() - 1)
			{
#if defined(MUON_PLATFORM_WINDOWS)
				printf(" %s |-", depth);
#else
				printf(" %s ├─", depth);
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
			displayNodeHierarchy(n);
			pop();
		}
#endif
	}

	/*
	* Custom SharedData
	* Specific for Lexical parsing
	*/
	struct InternalSyntaxicData
	{
		InternalSyntaxicData(hz::Error& err)
			: error(err)
		{
		}

		std::vector<hz::parser::Token>* tokenList;
		std::vector<hz::parser::Compiler::Section>* sections;
		hz::parser::ASTNode* rootNode;

		hz::Error& error;
		m::u32 readTokenIndex;

		hz::parser::Token lastReadToken;
		hz::parser::Token prevReadToken;

		std::vector<hz::parser::ASTNode*> exprValue;
		std::vector<hz::parser::ASTNode*> exprOperator;
		m::u32 openParenthesis;
		m::u32 openBracket;
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
			table[hz::parser::MATH_ASN] = OpInfo(10, ASSOC_RIGHT);			//'='
			table[hz::parser::MATH_ASN_ADD] = OpInfo(10, ASSOC_RIGHT);		//'+='
			table[hz::parser::MATH_ASN_SUB] = OpInfo(10, ASSOC_RIGHT);		//'-='
			table[hz::parser::MATH_ASN_MUL] = OpInfo(10, ASSOC_RIGHT);		//'/='
			table[hz::parser::MATH_ASN_DIV] = OpInfo(10, ASSOC_RIGHT);		//'*='
			table[hz::parser::MATH_ASN_MOD] = OpInfo(10, ASSOC_RIGHT);		//'%='
			table[hz::parser::MATH_ASN_AND] = OpInfo(10, ASSOC_RIGHT);		//'&='
			table[hz::parser::MATH_ASN_OR] = OpInfo(10, ASSOC_RIGHT);		//'|='
			table[hz::parser::MATH_ASN_XOR] = OpInfo(10, ASSOC_RIGHT);		//'^='
			table[hz::parser::MATH_ASN_NOT] = OpInfo(10, ASSOC_RIGHT);		//'~='

			table[hz::parser::LOGIC_OR] = OpInfo(25, ASSOC_LEFT);			//'||'

			table[hz::parser::LOGIC_AND] = OpInfo(30, ASSOC_LEFT);			//'&&'

			table[hz::parser::BITWISE_OR] = OpInfo(35, ASSOC_LEFT);			//'|'

			table[hz::parser::BITWISE_XOR] = OpInfo(40, ASSOC_LEFT);		//'^'

			table[hz::parser::BITWISE_AND] = OpInfo(50, ASSOC_LEFT);		//'&'

			table[hz::parser::LOGIC_EQ] = OpInfo(55, ASSOC_LEFT);			//'=='
			table[hz::parser::LOGIC_NEQ] = OpInfo(55, ASSOC_LEFT);			//'!='

			table[hz::parser::LOGIC_LT] = OpInfo(60, ASSOC_LEFT);			//'<'
			table[hz::parser::LOGIC_LET] = OpInfo(60, ASSOC_LEFT);			//'<='
			table[hz::parser::LOGIC_GT] = OpInfo(60, ASSOC_LEFT);			//'>'
			table[hz::parser::LOGIC_GET] = OpInfo(60, ASSOC_LEFT);			//'>='

			table[hz::parser::BITWISE_LSH] = OpInfo(65, ASSOC_LEFT);		//'<<'
			table[hz::parser::BITWISE_RSH] = OpInfo(65, ASSOC_LEFT);		//'>>'

			table[hz::parser::MATH_ADD] = OpInfo(70, ASSOC_LEFT);			//'+'
			table[hz::parser::MATH_SUB] = OpInfo(70, ASSOC_LEFT);			//'-'

			table[hz::parser::MATH_MUL] = OpInfo(75, ASSOC_LEFT);			//'*'
			table[hz::parser::MATH_DIV] = OpInfo(75, ASSOC_LEFT);			//'/'
			table[hz::parser::MATH_MOD] = OpInfo(75, ASSOC_LEFT);			//'%'

			table[hz::parser::UNARY_PLUS] = OpInfo(80, ASSOC_RIGHT);		//'+' before an expression (is optional: +5 <=> 5)
			table[hz::parser::UNARY_MINUS] = OpInfo(80, ASSOC_RIGHT);		//'-' before an expression (-5 is not parsed as 0-5)
			table[hz::parser::BITWISE_NOT] = OpInfo(80, ASSOC_RIGHT);		//'~'
			table[hz::parser::LOGIC_NOT] = OpInfo(80, ASSOC_RIGHT);			//'!'
			table[hz::parser::MATH_INC_PREFIX] = OpInfo(80, ASSOC_RIGHT);	//'++'var
			table[hz::parser::MATH_DEC_PREFIX] = OpInfo(80, ASSOC_RIGHT);	//'--'var

			table[hz::parser::MATH_INC_POSTFIX] = OpInfo(85, ASSOC_LEFT);	//var'++'
			table[hz::parser::MATH_DEC_POSTFIX] = OpInfo(85, ASSOC_LEFT);	//var'--'
			table[hz::parser::S_ACCESSOR] = OpInfo(85, ASSOC_LEFT);			//'.'
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
			impl->prevReadToken = impl->lastReadToken;
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
	bool mergeOperatorValue(InternalSyntaxicData*);
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
			impl.openParenthesis = 0;
			impl.openBracket = 0;

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
			displayNodeHierarchy(m_nodeRoot);
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
					auto* varNode = MUON_NEW(hz::parser::ASTNodeVariableManipulation);
					varNode->type = hz::parser::NT_GLOBAL_DECL;
					varNode->decl.type = typeName;
					varNode->decl.name = varName;
					varNode->decl.global = true;
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
		hz::parser::ASTNodeNamespaceDecl* namespaceNode = MUON_NEW(hz::parser::ASTNodeNamespaceDecl);
		namespaceNode->type = hz::parser::NT_NAMESPACE_DECL;

		if (readToken(impl, token) && token.type == hz::parser::V_IDENTIFIER)
		{
			popToken(impl);
			namespaceNode->decl.name = token.value.get<m::String>();
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
		functionNode->decl.type = token.value.get<m::String>();
		functionNode->type = hz::parser::NT_FUNCTION_DECL;
		impl->currNode->addChild(functionNode);
		impl->currNode = functionNode;

		if (readToken(impl, token) && token.type == hz::parser::V_IDENTIFIER)
		{
			popToken(impl);
			functionNode->decl.name = token.value.get<m::String>();
			// Parse arguments
			if (readToken(impl, token) && token.type == hz::parser::S_LPARENT)
			{
				popToken(impl);
				// Quick check for a non argument function
				if (readToken(impl, token) && token.type == hz::parser::S_RPARENT)
				{
					popToken(impl);
				}
				else
				{
					// Parse arguments
					if (parseArgsDecl(impl))
					{
						if (readToken(impl, token) && token.type == hz::parser::S_RPARENT)
						{
							popToken(impl);
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
				// Same, quick check for an empty body
				if (readToken(impl, token) && token.type == hz::parser::S_RBRACE)
				{
					popToken(impl);
					impl->currNode = functionNode->parent;
					return true;
				}
				else
				{
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
				m::String vartype = token.value.get<m::String>();
				readToken(impl, token);
				if (token.type == hz::parser::V_IDENTIFIER)
				{
					popToken(impl);

					hz::parser::DeclVariable declArg;
					declArg.prefix = prefix;
					declArg.type = vartype;
					declArg.name = token.value.get<m::String>();
					auto* funcNode = (hz::parser::ASTNodeFunctionDecl*)impl->currNode;
					funcNode->decl.params.push_back(declArg);

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
			auto* classNode = MUON_NEW(hz::parser::ASTNodeClassDecl);
			impl->currNode->addChild(classNode);
			impl->currNode = classNode;
			classNode->type = hz::parser::NT_CLASS_DECL;
			classNode->decl.name = token.value.get<m::String>();
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
		auto* argDecl = MUON_NEW(hz::parser::ASTNodeMemberDecl);
		argDecl->type = hz::parser::NT_CLASS_MEMBER;

		readToken(impl, token);
		popToken(impl);
		argDecl->decl.type = token.value.get<m::String>();

		readToken(impl, token);
		popToken(impl);
		argDecl->decl.name = token.value.get<m::String>();

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

		cdstrNode->decl.name = keyword;
		impl->currNode->addChild(cdstrNode);
		impl->currNode = cdstrNode;

		// Parse arguments
		if (readToken(impl, token) && token.type == hz::parser::S_LPARENT)
		{
			popToken(impl);
			// Quick check for a non argument function
			if (readToken(impl, token) && token.type == hz::parser::S_RPARENT)
			{
				popToken(impl);
			}
			else
			{
				// Parse arguments
				if (parseArgsDecl(impl))
				{
					if (readToken(impl, token) && token.type == hz::parser::S_RPARENT)
					{
						popToken(impl);
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
			// Same, quick check for an empty body
			if (readToken(impl, token) && token.type == hz::parser::S_RBRACE)
			{
				popToken(impl);
				impl->currNode = cdstrNode->parent;
				return true;
			}
			else
			{
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
		else if (impl->currNode->type == hz::parser::NT_FUNCTION_DECL
				 && token.type == hz::parser::S_RBRACE)
		{
			// Token will be poped by parseFunctionDecl, and current node impl too
			return true;
		}
		// Parse expression
		else if (parseExpr(impl, hz::parser::S_SEPARATOR))
		{
			goto label_statement_check_rec;
		}

		// An error occured somewhere
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
		if (readToken(impl, token))
		{
			popToken(impl);
			auto* retNode = MUON_NEW(hz::parser::ASTNode);
			retNode->type = hz::parser::NT_EXPR_RETURN;
			impl->currNode->addChild(retNode);
			impl->currNode = retNode;
			if (!parseExpr(impl, hz::parser::S_SEPARATOR))
			{
				return false;
			}
			impl->currNode = retNode->parent;
			return true;
		}

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

	bool mergeOperatorValue(InternalSyntaxicData* impl)
	{
		if (!impl->exprOperator.empty())
		{
			hz::parser::ASTNodeOperator* op = (hz::parser::ASTNodeOperator*)impl->exprOperator.back();
			if (op->type == hz::parser::S_ACCESSOR
				|| op->type == hz::parser::UNARY_MINUS
				|| op->type == hz::parser::UNARY_PLUS
				|| (op->type > hz::parser::E_BITWISE_OP_BEGIN && op->type < hz::parser::E_BITWISE_OP_END)
				|| (op->type > hz::parser::E_LOGIC_OP_BEGIN && op->type < hz::parser::E_LOGIC_OP_END)
				|| (op->type > hz::parser::E_MATH_OP_BEGIN && op->type < hz::parser::E_MATH_OP_END)
				|| (op->type > hz::parser::E_ASN_OP_BEGIN && op->type < hz::parser::E_ASN_OP_END))
			{
				if ((op->binop && impl->exprValue.size() >= 2)
					|| (!op->binop && impl->exprValue.size() >= 1))
				{
					auto* left = impl->exprValue.back();
					impl->exprValue.pop_back();
					op->addChild(left);
					if (op->binop)
					{
						auto* right = impl->exprValue.back();
						impl->exprValue.pop_back();
						op->addChild(right);
					}
					impl->exprValue.push_back(op);
					impl->exprOperator.pop_back();
					return true;
				}
			}
		}
		else if (impl->exprOperator.empty() && impl->exprValue.size() == 1)
		{
			impl->currNode->addChild(impl->exprValue.back());
			impl->exprValue.pop_back();
			return true;
		}

		return false;
	}

	bool parseExprNewVarDecl(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;
		readToken(impl, token);
		popToken(impl);

		auto* newNode = MUON_NEW(hz::parser::ASTNodeVariableManipulation);
		newNode->type = hz::parser::NT_EXPR_NEW;
		newNode->decl.global = false;
		impl->currNode->addChild(newNode);
		impl->currNode = newNode;

		if (readToken(impl, token) && token.type == hz::parser::V_IDENTIFIER)
		{
			popToken(impl);
			m::String vartype = token.value.get<m::String>();
			if (readToken(impl, token) && token.type == hz::parser::V_IDENTIFIER)
			{
				popToken(impl);
				newNode->decl.type = vartype;
				newNode->decl.name = token.value.get<m::String>();
				// Ok now, let's search for ';' (no value), '=' or '(' for constructor with value
				if (readToken(impl, token))
				{
					if (token.type == hz::parser::S_SEPARATOR)
					{
						popToken(impl);
						impl->currNode = newNode->parent;
						return true;
					}
					else if (token.type == hz::parser::MATH_ASN
							 || token.type == hz::parser::S_LPARENT)
					{
						// A bit of cheat here:
						// If instruction is like ' = expr; ', pop '='
						// so expression is 'expr;'
						// if instruction is ' ( expr );', then add here the '(' as function
						// so commas are allowed (pop it anyway so it's not added twice)
						popToken(impl);
						if (token.type == hz::parser::S_LPARENT)
						{
							++impl->openParenthesis;
							auto* lparentNode = MUON_NEW(hz::parser::ASTNodeParenthesis);
							lparentNode->type = hz::parser::S_LPARENT;
							lparentNode->exprValueIndex = impl->exprValue.size();
							lparentNode->funcIdentNode = newNode;
							impl->exprOperator.push_back(lparentNode);
						}

						if (parseExpr(impl, hz::parser::S_SEPARATOR))
						{
							// And as Expr have parsed the expression as it's a func call, must re-update type
							// so it's really a new var declaration
							newNode->type = hz::parser::NT_EXPR_NEW;
							impl->currNode = newNode->parent;
							return true;
						}
					}
				}
			}
		}

		tokenError(impl, token);
		return false;
	}

	bool parseExprDeleteVarDecl(InternalSyntaxicData* impl)
	{
		hz::parser::Token token;
		readToken(impl, token);
		popToken(impl);

		auto* delNode = MUON_NEW(hz::parser::ASTNodeVariableManipulation);
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
				while (canMergeOperatorValue(impl))
				{
					if (!mergeOperatorValue(impl))
					{
						tokenError(impl, token);
						return false;
					}
				}
				return true;
			}

			// Comma is only allowed if surrounded by parenthesis: (Expr, Expr, ...)
			if (token.type == hz::parser::S_COMMA)
			{
				bool found = false;
				while (canMergeOperatorValue(impl))
				{
					auto* op = impl->exprOperator.back();
					if (op->type == hz::parser::S_LPARENT)
					{
						auto* lparentNode = (hz::parser::ASTNodeParenthesis*)op;
						if (lparentNode->funcIdentNode)
						{
							found = true;
							break;
						}
						else
						{
							tokenError(impl, token, "',' found outside a function call!");
							return false;
						}
					}
					else if (!mergeOperatorValue(impl))
					{
						tokenError(impl, token);
						return false;
					}
				}
				if (!found)
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
				constNode->decl.value = token.value;
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
			else if (token.type == hz::parser::S_LPARENT)
			{
				++impl->openParenthesis;
				auto* lparentNode = MUON_NEW(hz::parser::ASTNodeParenthesis);
				lparentNode->type = hz::parser::S_LPARENT;
				lparentNode->exprValueIndex = impl->exprValue.size();
				lparentNode->funcIdentNode = NULL;
				// If previous Token is an Identifier, then replace by a ASTNodeFunctionCall
				if (impl->prevReadToken.type == hz::parser::V_IDENTIFIER)
				{
					auto* funcNode = MUON_NEW(hz::parser::ASTNodeFunctionCall);
					auto* oldIdent = impl->exprValue.back();
					funcNode->name = oldIdent->name;
					lparentNode->funcIdentNode = funcNode;
					impl->exprValue.back() = funcNode;
					MUON_DELETE(oldIdent);
				}
				impl->exprOperator.push_back(lparentNode);
			}
			else if (token.type == hz::parser::S_RPARENT)
			{
				if (impl->openParenthesis > 0)
				{
					// We have an open parenthesis somewhere, merge until we found it.
					// Also, if previous Value is Identifier, then it means
					// we have a function call
					--impl->openParenthesis;
					auto* op = impl->exprOperator.back();
					bool found = false;
					hz::parser::ASTNode* funcIdentNode = NULL;
					m::u32 exprValueIndex = 0;
					while (!found && op != NULL)
					{
						if (op->type == hz::parser::S_LPARENT)
						{
							found = true;
							funcIdentNode = ((hz::parser::ASTNodeParenthesis*)op)->funcIdentNode;
							exprValueIndex = ((hz::parser::ASTNodeParenthesis*)op)->exprValueIndex;
							impl->exprOperator.pop_back();
							MUON_DELETE(op);
						}
						else
						{
							if (!canMergeOperatorValue(impl) || !mergeOperatorValue(impl))
							{
								tokenError(impl, token);
								return false;
							}
						}
						op = (impl->exprOperator.empty() ? NULL : impl->exprOperator.back());
					}

					// All parenthesis subexpr have been parsed
					if (funcIdentNode)
					{
						funcIdentNode->type = hz::parser::NT_EXPR_FUNC_CALL;
						// Extract as much Node as its supposed param count
						// (They're in reverse order)
						while (impl->exprValue.size() != exprValueIndex)
						{
							funcIdentNode->addChild(impl->exprValue.back());
							impl->exprValue.pop_back();
						}

						// Reverse children in correct order
						m::u32 end = funcIdentNode->children->size() - 1;
						for (m::u32 i = 0; i < funcIdentNode->children->size() / 2; ++i)
						{
							auto* n = (*funcIdentNode->children)[i];
							(*funcIdentNode->children)[i] = (*funcIdentNode->children)[end - i];
							(*funcIdentNode->children)[end - i] = n;
						}
					}
				}
				else
				{
					tokenError(impl, token, "Closing parenthesis has been found, but missing the open one!");
					return false;
				}
			}
			// Operator
			else
			{
				auto* opNode = MUON_NEW(hz::parser::ASTNodeOperator);
				opNode->binop = true;
				opNode->type = token.type;
				OpInfo currInfo = s_PrecedenceTable[token.type];

				hz::parser::eTokenType ltype = impl->prevReadToken.type;
				// Check for Unary operators:
				// Prefix unary operator (!var ~var ++var --var +var -var)
				// Last token is either binop, unop, comma, ( or [
				if ((token.type == hz::parser::LOGIC_NOT
					|| token.type == hz::parser::BITWISE_NOT
					|| token.type == hz::parser::MATH_ADD
					|| token.type == hz::parser::MATH_SUB
					|| token.type == hz::parser::MATH_INC_DEFAULT
					|| token.type == hz::parser::MATH_DEC_DEFAULT)
					&&
					(ltype == hz::parser::S_COMMA
					|| ltype == hz::parser::S_LPARENT
					|| ltype == hz::parser::S_LBRACKET
					|| ltype == hz::parser::UNARY_MINUS
					|| ltype == hz::parser::UNARY_PLUS
					|| (ltype > hz::parser::E_ASN_OP_BEGIN && ltype < hz::parser::E_ASN_OP_END)
					|| (ltype > hz::parser::E_MATH_OP_BEGIN && ltype < hz::parser::E_MATH_OP_END)
					|| (ltype > hz::parser::E_LOGIC_OP_BEGIN && ltype < hz::parser::E_LOGIC_OP_END)
					|| (ltype > hz::parser::E_BITWISE_OP_BEGIN && ltype < hz::parser::E_BITWISE_OP_END)))
				{
					opNode->binop = false;

					if (token.type == hz::parser::MATH_INC_DEFAULT || token.type == hz::parser::MATH_DEC_DEFAULT)
					{
						opNode->type = (token.type == hz::parser::MATH_INC_DEFAULT ?
										hz::parser::MATH_INC_PREFIX : hz::parser::MATH_DEC_PREFIX);
					}
					else
					{
						opNode->type = (token.type == hz::parser::MATH_SUB ? hz::parser::UNARY_MINUS
										: token.type == hz::parser::MATH_ADD ? hz::parser::UNARY_PLUS
										: opNode->type);
					}
				}
				// Postfix unary operator (var++ var--)
				// Last token is either identifier, number, ) or ]
				else if ((token.type == hz::parser::MATH_INC_DEFAULT
					|| token.type == hz::parser::MATH_DEC_DEFAULT)
					&&
					(ltype == hz::parser::V_IDENTIFIER
					|| ltype == hz::parser::V_NUMBER
					|| ltype == hz::parser::S_LPARENT
					|| ltype == hz::parser::S_LBRACKET))
				{
					opNode->binop = false;
					opNode->type = (token.type == hz::parser::MATH_INC_DEFAULT ?
									hz::parser::MATH_INC_POSTFIX : hz::parser::MATH_DEC_POSTFIX);
				}

				bool reduce = !impl->exprOperator.empty();
				while (reduce && opNode->binop)
				{
					auto* prevNode = impl->exprOperator.back();
					OpInfo prevInfo = s_PrecedenceTable[prevNode->type];
					// We need to reduce if previous operator as a right associativity and current one is left,
					// or if the previous operator as a higher precedence than current.
					// We don't need to merge in other cases
					// Also, we don't reduce if '(' is encountered, that's the role of the ')' code
					bool diffAssoc = ((prevInfo.precedence >= currInfo.precedence)
									  && (prevInfo.associativity == ASSOC_RIGHT && currInfo.associativity == ASSOC_LEFT));
					bool precOnly = (prevInfo.precedence > currInfo.precedence);
					if (diffAssoc || precOnly
						&& prevNode->type != hz::parser::S_LPARENT)
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
