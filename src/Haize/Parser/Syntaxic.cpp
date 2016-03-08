
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
		PHASE_ROOT,
		PHASE_EXPRESSION,
		PHASE_CLASS,
		PHASE_FUNCTION,
		PHASE_NAMESPACE,
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

		std::deque<hz::parser::Token> exprTokens;
		std::deque<hz::parser::ASTNode*> exprNodes;
		hz::parser::ASTNode* phaseNode;

		static const OpAttribute precAttrib[hz::parser::E_TERMINALTOKEN_END];
	};
}

namespace hz
{
	bool Context::readToken(parser::Token& out, bool pop)
	{
		if(!m_tokenList->empty())
		{
			out = m_tokenList->back();
			if(pop)
			{
				m_tokenList->pop_back();
			}
			return true;
		}
		return false;
	}

	bool Context::parseSyntaxic()
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

		InfoSyntaxic impl;
		impl.phases.push_back(PHASE_ROOT);
		impl.readIndex = 0;
		impl.phaseNode = m_nodeRoot;

		// Skip empty Token list
		if (m_tokenList->empty()
			|| m_tokenList->back().type == parser::S_EOF)
		{
			return true;
		}

		while(!m_tokenList->empty())
		{
			if(!parseChunk(&impl))
			{
				return false;
			}
		}

		clearError(true);
		return true;
	}

	bool Context::parseChunk(parser::InfoImpl* info)
	{
		bool ok = true;
		InfoSyntaxic* impl = (InfoSyntaxic*)info;
		parser::Token token = m_tokenList->back();
		ParserPhase phase = impl->phases.back();

		// Pop token that have no real value
		if (token.type == parser::S_SEPARATOR
			|| token.type == parser::S_EOF)
		{
			m_tokenList->pop_back();
			return true;
		}

		// It may be a closing from a previous parse, check it here
		if (token.type == parser::S_RBRACE)
		{
			if (impl->phases.empty())
			{
				tokenError(token, "Unexpected token \"" + token.value.get<m::String>() + "\"");
				return false;
			}

			switch (phase)
			{
				case PHASE_CLASS:
				case PHASE_FUNCTION:
				case PHASE_NAMESPACE:
				{
					impl->phases.pop_back();
					m_tokenList->pop_back();
					return true;
				}
				default:
				{
					tokenError(token, "Unexpected token \"" + token.value.get<m::String>() + "\"");
					return false;
				}
			}
		}

		// Check phase to call the correct parse phase
		switch(phase)
		{
			case PHASE_ROOT:
			{
				if (token.type == parser::S_KEYWORD)
				{
					m::String keyword = token.value.get<m::String>();
					if (keyword == "class")
					{
						ok = parseClass(info);
					}
					else if (keyword == "namespace")
					{
						ok = parseNamespace(info);
					}
					else if (keyword == "global")
					{
						ok = parseGlobal(info);
					}
					else
					{
						tokenError(token, "Unexpected keyword \"" + keyword + "\"");
						return false;
					}
				}
				// Else if identifier, it can only be a function
				else if (token.type == parser::V_IDENTIFIER)
				{
					ok = parseFunction(info);
				}
				else
				{
					m::String tokstr = (MUON_META(m::String) == token.value.getMeta() ? token.value.get<m::String>() : parser::TokenTypeStr[token.type]);
					tokenError(token, "Unexpected token \"" + tokstr + "\"");
					return false;
				}
				break;
			}
			case PHASE_FUNCTION:
			{
				ok = parseExpression(info);
				break;
			}
			case PHASE_NAMESPACE:
			{
				if (token.type == parser::S_KEYWORD
					&& token.value.get<m::String>() == "namespace")
				{
					ok = parseNamespace(info);
				}
				else
				{
					ok = parseChunk(info);
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

	bool Context::parseExpression(parser::InfoImpl* info)
	{
		InfoSyntaxic* impl = (InfoSyntaxic*)info;
		parser::Token token = m_tokenList->back();
		if (token.category == parser::CATEGORY_BINOP)
		{
			// If we've less than 2 variable on the left, there is a problem
			if (m_tokenList->size() < 2)
			{
				tokenError(token, "Operator \"" + token.value.get<m::String>() + "\" has not enough operand!");
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
		parser::Token token;
		bool ok = true;

		return true;
	}

	bool Context::parseNamespace(parser::InfoImpl* info)
	{
		InfoSyntaxic* impl = (InfoSyntaxic*)info;
		parser::Token token;
		bool ok = true;

		m_tokenList->pop_back();
		ok = readToken(token, true);
		if(!ok || token.type != parser::V_IDENTIFIER)
		{
			tokenError(token, "Expected identifier after 'namespace'!");
			return false;
		}

		// Update phase
		impl->phases.push_back(PHASE_NAMESPACE);
		parser::ASTNode* namespaceNode = MUON_NEW(parser::ASTNode, parser::NT_NAMESPACE, token.value.get<m::String>());
		impl->phaseNode->addChild(namespaceNode);

		// Set namespace as phaseNode
		impl->phaseNode = namespaceNode;

		// {
		ok = readToken(token, true);
		if(!ok || token.type != parser::S_LBRACE)
		{
			tokenError(token, "Missing '{' after namespace declaration!");
			return false;
		}
		return true;
	}

	bool Context::parseClass(parser::InfoImpl* info)
	{
		InfoSyntaxic* impl = (InfoSyntaxic*)info;
		parser::Token token;
		bool ok = true;

		// Extract class keyword
		m_tokenList->pop_back();

		// Update phase
		parser::ASTNode* classNode = MUON_NEW(parser::ASTNode, parser::NT_CLASS, "#NT_CLASS#");
		impl->phaseNode->addChild(classNode);

		// Check function identifer (the function name)
		ok = readToken(token, true);
		if(ok && token.type == parser::V_IDENTIFIER)
		{
			classNode->name = token.value.get<m::String>();
		}
		else
		{
			tokenError(token, "Expected identifier for class declaration!");
			return false;
		}

		return true;
	}

	bool Context::parseFunction(parser::InfoImpl* info)
	{
		InfoSyntaxic* impl = (InfoSyntaxic*)info;
		parser::Token token;
		bool ok;

		// Update phase
		parser::ASTNode* funcNode = MUON_NEW(parser::ASTNode, parser::NT_FUNCTION, "#NT_FUNCTION#");
		impl->phaseNode->addChild(funcNode);

		// Current token is "return type", pop it,
		// and create required nodes
		readToken(token, true);
		funcNode->addChild(MUON_NEW(parser::ASTNode, token));

		// Check function identifer (the function name)
		ok = readToken(token, true);
		if(ok && token.type == parser::V_IDENTIFIER)
		{
			funcNode->name = token.value.get<m::String>();
		}
		else
		{
			tokenError(token, "Expected identifier for function declaration!");
			return false;
		}

		// (
		ok = readToken(token, true);
		if(!ok || token.type != parser::S_LPARENT)
		{
			tokenError(token, "Missing '(' after function name!");
			return false;
		}

		// Check arguments, they follow the EBNF pattern:
		// [ V_IDENTIFIER V_IDENTIFIER {S_COMMA V_IDENTIFIER V_IDENTIFIER [S_COMMA]}]
		// (the reason for the double identifier is because "varType varName", a trailing comma is allowed)
		// Just check we have at least something, else directly jump to ')'
		{
			//In any cases, push a NT_FUNCTION_ARGS node
			parser::ASTNode* args = MUON_NEW(parser::ASTNode, parser::NT_FUNCTION_ARGS, "NT_FUNCTION_ARGS");
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
				ok = readToken(token, false);
				if(!ok)
				{
					tokenError(token, "Expected token for function argument list!");
					return false;
				}

				if (token.type != parser::S_RPARENT)
				{
					if(token.type == parser::S_KEYWORD)
					{
						m::String keyword = token.value.get<m::String>();
						if(keyword == "ref")
						{
							refNode = MUON_NEW(parser::ASTNode, token);
							m_tokenList->pop_back(); // remove 'ref'
							ok = readToken(token, false);
							if(!ok)
							{
								tokenError(token, "Expected return type after 'ref' keyword!");
								return false;
							}
						}
						else
						{
							tokenError(token, "Unexpected token for function argument! (Expected ref, got something else)");
							return false;
						}
					}

					if(token.type == parser::V_IDENTIFIER)
					{
						retType = MUON_NEW(parser::ASTNode, token);
						m_tokenList->pop_back(); // remove rettype

						ok = readToken(token, true);
						if(!ok || token.type != parser::V_IDENTIFIER)
						{
							tokenError(token, "Expected variable name for function argument!");
							return false;
						}

						varName = MUON_NEW(parser::ASTNode, token);
						varName->addChild(retType);

						// Either add refNode or directly varName
						args->addChild(refNode ? (refNode->addChild(varName), refNode) : varName);
					}
					else
					{
						tokenError(token, "Unexpected token in function argument list!");
						return false;
					}

					ok = readToken(token, false);
					if(!ok)
					{
						tokenError(token, "Unexpected EOF in function argument list!");
						return false;
					}
					else if(token.type == parser::S_COMMA)
					{
						// Just consume it.
						// This allow an argument list ending by a ,
						m_tokenList->pop_back();
					}
				}
			} while (token.type != parser::S_RPARENT);
		}

		// )
		ok = readToken(token, true);
		if(!ok || token.type != parser::S_RPARENT)
		{
			tokenError(token, "Missing ')' to end function parameter list!");
			return false;
		}

		// {
		ok = readToken(token, true);
		if(!ok || token.type != parser::S_LBRACE)
		{
			tokenError(token, "Missing '{' after function parameter list!");
			return false;
		}

		// Update phase only if successful
		impl->phases.push_back(PHASE_FUNCTION);
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

