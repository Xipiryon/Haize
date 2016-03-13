#include <Muon/System/Log.hpp>

#include "Haize/Parser/Priv/DefaultCompiler.hpp"

namespace
{
	m::String unexpectedToken(const hz::parser::Token& token)
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
		return m::String("Unexpected token \"" + tokstr + "\"");
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

	enum ParserPhase
	{
		PHASE_ROOT,
		PHASE_EXPRESSION,
		PHASE_CLASS,
		PHASE_FUNCTION,
		PHASE_NAMESPACE,
	};

	/*
	* Custom SharedData
	* Specific for Lexical parsing
	*/
	struct SharedDataSyntaxic : public hz::parser::priv::SharedData
	{
		SharedDataSyntaxic(hz::Error& err)
			: error(err)
		{
		}

		virtual ~SharedDataSyntaxic()
		{
		}

		hz::Error& error;
		std::deque<ParserPhase> phases;
		m::u32 readIndex;

		std::deque<hz::parser::Token> exprTokens;
		std::deque<hz::parser::ASTNode*> exprNodes;
		hz::parser::ASTNode* phaseNode;
		bool funcCstrDstr;

		static const OpAttribute precAttrib[hz::parser::E_TERMINALTOKEN_END];
	};
}

namespace hz
{
	namespace parser
	{
		namespace priv
		{
			bool DefaultCompiler::readToken(parser::Token& out, m::u32 index)
			{
				if (index < m_tokenList->size())
				{
					out = (*m_tokenList)[m_tokenList->size() - (index + 1)];
					return true;
				}
				return false;
			}

			bool DefaultCompiler::popToken(m::u32 count)
			{
				if (count <= m_tokenList->size())
				{
					for (m::u32 i = 0; i < count; ++i)
					{
						m_tokenList->pop_back();
					}
					return true;
				}
				return false;
			}

			void DefaultCompiler::tokenError(Error& error, const parser::Token& token, const m::String& msg)
			{
				error.clear();
				error.step = Error::COMPILATION;
				error.state = Error::ERROR;
				error.section = token.section;
				error.line = token.line;
				error.column = token.column;
				error.message = msg;
			}

			bool DefaultCompiler::syntaxic(Error& error)
			{
				// Clean children
				while (!m_nodeRoot->children->empty())
				{
					MUON_DELETE(m_nodeRoot->children->back());
					m_nodeRoot->children->pop_back();
				}

				// Reinit token
				m_nodeRoot->name = "#ROOT#";
				m_nodeRoot->token.type = parser::NT_ROOT;

				SharedDataSyntaxic impl(error);
				impl.phases.push_back(PHASE_ROOT);
				impl.readIndex = 0;
				impl.phaseNode = m_nodeRoot;
				impl.funcCstrDstr = false;

				// Skip empty Token list
				if (m_tokenList->empty()
					|| m_tokenList->back().type == parser::S_EOF)
				{
					return true;
				}

				while (!m_tokenList->empty())
				{
					bool ok = true;
					parser::Token token = m_tokenList->back();
					ParserPhase phase = impl.phases.back();

					// Pop token that have no real value
					if (token.type == parser::S_SEPARATOR
						|| token.type == parser::S_EOF)
					{
						popToken(1);
						return true;
					}

					// It may be a closing from a previous parse, check it here
					if (token.type == parser::S_RBRACE)
					{
						if (impl.phases.empty())
						{
							tokenError(error, token, "Unexpected token \"" + token.value.get<m::String>() + "\"");
							return false;
						}

						impl.phases.pop_back();
						popToken(1);
						return true;
					}

					// Check phase to call the correct parse phase
					switch (phase)
					{
						case PHASE_ROOT:
						{
							if (token.type == parser::S_KEYWORD)
							{
								m::String keyword = token.value.get<m::String>();
								if (keyword == "class")
								{
									ok = parseClass(&impl);
								}
								else if (keyword == "namespace")
								{
									ok = parseNamespace(&impl);
								}
								else if (keyword == "global")
								{
									ok = parseGlobal(&impl);
								}
								else
								{
									tokenError(error, token, "Unexpected keyword \"" + keyword + "\"");
									return false;
								}
							}
							// Else if identifier, it can only be a function
							else if (token.type == parser::V_IDENTIFIER)
							{
								ok = parseFunction(&impl);
							}
							else
							{
								tokenError(error, token, unexpectedToken(token));
								return false;
							}
							break;
						}
						case PHASE_CLASS:
						{
							parser::Token retType;
							parser::Token identifier;
							ok = readToken(retType, 0); // return type, or constructor/destructor
							if (ok)
							{
								if (retType.type == parser::S_KEYWORD)
								{
									m::String keyword = retType.value.get<m::String>();
									if (keyword == "constructor" || keyword == "destructor")
									{
										impl.funcCstrDstr = true;
										ok = parseFunction(&impl);
										impl.funcCstrDstr = false;
										break;
									}
									else
									{
										ok = false;
										break;
									}
								}
								else if (retType.type != parser::V_IDENTIFIER)
								{
									ok = false;
									break;
								}

								ok = readToken(identifier, 1); // member/function name
								ok &= identifier.type == parser::V_IDENTIFIER;
								if (ok)
								{
									ok = readToken(token, 2);
									// ( means a function
									// ; means an attribute
									// else, error
									switch (token.type)
									{
										case parser::S_LPARENT:
										{
											ok = parseFunction(&impl);
											break;
										}
										case parser::S_SEPARATOR:
										{
											parser::ASTNode* attribNode = MUON_NEW(parser::ASTNode, parser::NT_CLASS_MEMBER, identifier.value.get<m::String>());
											attribNode->addChild(MUON_NEW(parser::ASTNode, retType));
											impl.phaseNode->addChild(attribNode);
											popToken(3); // retType identifier separator
											break;
										}
										default:
										{
											ok = false;
											break;
										}
									}
								}
							}

							if (!ok)
							{
								tokenError(error, token, unexpectedToken(token));
								return false;
							}

							break;
						}
						case PHASE_FUNCTION:
						{
							ok = parseExpression(&impl);
							break;
						}
						case PHASE_NAMESPACE:
						{
							if (token.type == parser::S_KEYWORD)
							{
								m::String keyword = token.value.get<m::String>();
								if (keyword == "namespace")
								{
									ok = parseNamespace(&impl);
								}
								else if (keyword == "class")
								{
									ok = parseClass(&impl);
								}
							}
							else
							{
								ok = parseFunction(&impl);
							}
							break;
						}
					}

					if (!ok)
					{
						// Error have been set in sub syntaxic parseN functions
						return false;
					}

					return true;
				}
				error.clear();
				return true;
			}

			bool DefaultCompiler::parseGlobal(SharedData* info)
			{
				SharedDataSyntaxic* impl = (SharedDataSyntaxic*)info;
				parser::Token token;
				bool ok = true;

				return true;
			}

			bool DefaultCompiler::parseNamespace(SharedData* info)
			{
				SharedDataSyntaxic* impl = (SharedDataSyntaxic*)info;
				parser::Token token;
				bool ok = true;

				popToken(1);
				ok = readToken(token, 0);
				popToken(1);
				if (!ok || token.type != parser::V_IDENTIFIER)
				{
					tokenError(impl->error, token, "Expected identifier after 'namespace'!");
					return false;
				}

				// Update phase
				impl->phases.push_back(PHASE_NAMESPACE);
				parser::ASTNode* namespaceNode = MUON_NEW(parser::ASTNode, parser::NT_NAMESPACE, token.value.get<m::String>());
				impl->phaseNode->addChild(namespaceNode);

				// Set namespace as phaseNode
				impl->phaseNode = namespaceNode;

				// {
				ok = readToken(token, 0);
				popToken(1);
				if (!ok || token.type != parser::S_LBRACE)
				{
					tokenError(impl->error, token, "Missing '{' after namespace declaration!");
					return false;
				}
				return true;
			}

			bool DefaultCompiler::parseClass(SharedData* info)
			{
				SharedDataSyntaxic* impl = (SharedDataSyntaxic*)info;
				parser::Token token;
				bool ok = true;

				// Extract class keyword
				popToken(1);

				// Update phase
				parser::ASTNode* classNode = MUON_NEW(parser::ASTNode, parser::NT_CLASS_DECL);

				// Check function identifer (the function name)
				ok = readToken(token, 0);
				popToken(1);
				if (ok && token.type == parser::V_IDENTIFIER)
				{
					classNode->name = token.value.get<m::String>();
					ok = readToken(token, 0);
					popToken(1);
					if (ok && token.type == parser::S_LBRACE)
					{
						impl->phaseNode->addChild(classNode);
						impl->phaseNode = classNode;
						impl->phases.push_back(PHASE_CLASS);
					}
				}
				else
				{
					MUON_DELETE(classNode);
					tokenError(impl->error, token, "Expected identifier for class declaration!");
					return false;
				}

				return true;
			}

			bool DefaultCompiler::parseFunction(SharedData* info)
			{
				SharedDataSyntaxic* impl = (SharedDataSyntaxic*)info;
				parser::Token token;
				bool ok;
				bool dstr = false;

				// Update phase
				parser::ASTNode* funcNode = MUON_NEW(parser::ASTNode, parser::NT_FUNCTION_DECL);

				// Current token is a return type, "constructor" or "destructor", pop it,
				// and create required nodes
				readToken(token, 0);
				popToken(1);
				funcNode->addChild(MUON_NEW(parser::ASTNode, token));

				// update node info if constructor/destructor
				// else look for function name
				if (impl->funcCstrDstr)
				{
					m::String keyword = token.value.get<m::String>();
					funcNode->name = keyword;
					if (keyword == "constructor")
					{
						funcNode->token.type = parser::NT_CLASS_CONSTRUCTOR;
					}
					else if (keyword == "destructor")
					{
						funcNode->token.type = parser::NT_CLASS_DESTRUCTOR;
						dstr = true;
					}
					else
					{
						tokenError(impl->error, token, unexpectedToken(token));
						return false;
					}
				}
				else
				{
					// Check function identifer (the function name)
					ok = readToken(token, 0);
					popToken(1);
					if (ok && token.type == parser::V_IDENTIFIER)
					{
						funcNode->name = token.value.get<m::String>();
					}
					else
					{
						tokenError(impl->error, token, "Expected identifier for function declaration!");
						return false;
					}
				}

				// (
				ok = readToken(token, 0);
				popToken(1);
				if (!ok || token.type != parser::S_LPARENT)
				{
					tokenError(impl->error, token, "Missing '(' after function name!");
					return false;
				}

				// Check arguments, unless where currently parsing a destructor
				if (!dstr)
				{
					//In any cases, push a NT_FUNCTION_ARGS node
					parser::ASTNode* args = MUON_NEW(parser::ASTNode, parser::NT_FUNCTION_ARGS);
					funcNode->addChild(args);

					parser::ASTNode* refNode;
					parser::ASTNode* retType;
					parser::ASTNode* varName;
					do
					{
						refNode = NULL;
						retType = NULL;
						varName = NULL;

						// read token without poping it
						ok = readToken(token, 0);
						if (!ok)
						{
							MUON_DELETE(funcNode);
							tokenError(impl->error, token, "Expected token for function argument list!");
							return false;
						}

						if (token.type != parser::S_RPARENT)
						{
							if (token.type == parser::S_KEYWORD)
							{
								m::String keyword = token.value.get<m::String>();
								if (keyword == "ref")
								{
									refNode = MUON_NEW(parser::ASTNode, token);
									popToken(1); // remove 'ref'
									ok = readToken(token, 0);
									if (!ok)
									{
										MUON_DELETE(funcNode);
										tokenError(impl->error, token, "Expected return type after 'ref' keyword!");
										return false;
									}
								}
								else
								{
									MUON_DELETE(funcNode);
									tokenError(impl->error, token, "Unexpected token for function argument! (Expected ref, got something else)");
									return false;
								}
							}

							if (token.type == parser::V_IDENTIFIER)
							{
								retType = MUON_NEW(parser::ASTNode, token);
								popToken(1); // remove rettype

								ok = readToken(token, 0);
								popToken(1);
								if (!ok || token.type != parser::V_IDENTIFIER)
								{
									MUON_DELETE(funcNode);
									tokenError(impl->error, token, "Expected variable name for function argument!");
									return false;
								}

								varName = MUON_NEW(parser::ASTNode, token);
								varName->addChild(retType);

								// Either add refNode or directly varName
								args->addChild(refNode ? (refNode->addChild(varName), refNode) : varName);
							}
							else
							{
								MUON_DELETE(funcNode);
								tokenError(impl->error, token, "Unexpected token in function argument list!");
								return false;
							}

							ok = readToken(token, 0);
							if (!ok)
							{
								MUON_DELETE(funcNode);
								tokenError(impl->error, token, "Unexpected EOF in function argument list!");
								return false;
							}
							else if (token.type == parser::S_COMMA)
							{
								// Just consume it.
								// This allow an argument list ending by a ,
								popToken(1);
							}
						}
					} while (token.type != parser::S_RPARENT);
				}
				else
				{
					ok = readToken(token, 0);
					if (!ok || token.type == parser::S_RPARENT)
					{
						tokenError(impl->error, token, unexpectedToken(token));
						return false;
					}
				}

				// )
				ok = readToken(token, 0);
				popToken(1);
				if (!ok || token.type != parser::S_RPARENT)
				{
					MUON_DELETE(funcNode);
					tokenError(impl->error, token, "Missing ')' to end function parameter list!");
					return false;
				}

				// {
				ok = readToken(token, 0);
				popToken(1);
				if (!ok || token.type != parser::S_LBRACE)
				{
					MUON_DELETE(funcNode);
					tokenError(impl->error, token, "Missing '{' after function parameter list!");
					return false;
				}

				// Update phase only if successful
				impl->phaseNode->addChild(funcNode);
				impl->phaseNode = funcNode;
				impl->phases.push_back(PHASE_FUNCTION);
				return true;
			}

			bool DefaultCompiler::parseExpression(SharedData* info)
			{
				SharedDataSyntaxic* impl = (SharedDataSyntaxic*)info;
				parser::Token token;
				bool ok = true;
				bool parseExpr = true;

				m::u32 openParenthesis = 0;
				m::u32 openBraces = 0;
				m::u32 openBrackets = 0;

				parser::ASTNode* exprRootNode = MUON_NEW(parser::ASTNode, parser::NT_EXPR);
				impl->phaseNode->addChild(exprRootNode);
				impl->phases.push_back(PHASE_EXPRESSION);
				impl->phaseNode = exprRootNode;

				// Allowed tokens are identifier, unary and binary operator, or keyword (control flow)
				while (ok && parseExpr)
				{
					ok = readToken(token, 0);
					// don't even parse if token are missing
					if (!ok)
					{
						continue;
					}

					// Create the AST
					if (token.type == parser::S_KEYWORD)
					{
						m::String keyword = token.value.get<m::String>();
						if (keyword == "return")
						{
							parser::ASTNode* node = MUON_NEW(parser::ASTNode, parser::NT_EXPR_RETURN);
							node->token = token;
							impl->phaseNode->addChild(node);
							impl->phaseNode = node;
						}
						else if (keyword == "new")
						{
							parser::ASTNode* node = MUON_NEW(parser::ASTNode, parser::NT_EXPR_CSTR);
							node->token = token;
							impl->phaseNode->addChild(node);
							impl->phaseNode = node;
						}
						else if (keyword == "delete")
						{
							parser::ASTNode* node = MUON_NEW(parser::ASTNode, parser::NT_EXPR_DSTR);
							node->token = token;
							impl->phaseNode->addChild(node);
							impl->phaseNode = node;
						}
						else
						{
							ok = false;
							continue;
						}
					}
					else if (token.type >= parser::E_VALUE_BEGIN
							 && token.type <= parser::E_VALUE_END)
					{
						if (token.type == parser::V_IDENTIFIER)
						{
						}
						else
						{
							parser::ASTNode* node = MUON_NEW(parser::ASTNode, token);
							impl->phaseNode->addChild(node);
						}
					}
					// Closing instructions
					else if (token.type == parser::S_SEPARATOR)
					{
						// Return to parent node
						impl->phaseNode = impl->phaseNode->parent;
					}
					// Closing braces
					else if (token.type == parser::S_RBRACE)
					{
						if (openBraces == 0)
						{
							parseExpr = false;
							continue;
						}
						--openBraces;
					}
					// Other unexpected tokens
					else
					{
						ok = false;
						continue;
					}

					popToken(1);
				}
				/*
				if (token.category == parser::CATEGORY_BINOP)
				{
				// If we've less than 2 variable on the left, there is a problem
				if (m_tokenList->size() < 2)
				{
				tokenError(error, token, "Operator \"" + token.value.get<m::String>() + "\" has not enough operand!");
				return false;
				}
				// old code
				//node = INFO_IMPL->stackValue[i];
				//node->addChild(INFO_IMPL->stackValue[i - 2]);
				//node->addChild(INFO_IMPL->stackValue[i - 1]);
				//INFO_IMPL->stackValue.erase(INFO_IMPL->stackValue.begin() + (--i)); // erase right
				//INFO_IMPL->stackValue.erase(INFO_IMPL->stackValue.begin() + (--i)); // erase left
				}
				// */
				if (!ok)
				{
					tokenError(impl->error, token, unexpectedToken(token));
					return false;
				}

				return true;
			}
		}
	}
}

const OpAttribute SharedDataSyntaxic::precAttrib[hz::parser::E_TERMINALTOKEN_END] =
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