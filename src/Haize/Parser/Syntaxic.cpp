
#include <Muon/System/Log.hpp>

#include "Haize/Context.hpp"

namespace
{
	enum ParserState
	{
		DONE,
		RUNNING,
		SKIPPING,
		ERROR,
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

		ParserState state;
		muon::u32 readIndex;
		hz::parser::ASTNode* node;

		std::deque<hz::parser::Token> operatorToken;
		std::deque<hz::parser::ASTNode*> exprNodes;
	};
}

namespace hz
{
	eCompilationState Context::parseSyntaxic()
	{
		while (!m_nodeRoot->children->empty())
		{
			MUON_DELETE(m_nodeRoot->children->back());
		}

		m_nodeRoot->name = "NT_CHUNK";
		m_nodeRoot->token = parser::Token(parser::NT_CHUNK);

		InfoSyntaxic impl;
		impl.node = NULL;
		impl.readIndex = 0;

		/*
		if (info.TokenList->empty() || info.TokenList->front().type == S_EOF)
		{
			info.error.message = "Empty token list, nothing to parse.";
			ret = false;
		}
		else
		{
			initPrecedenceAssoc(info);
			ret = initParse(info);
		}
		// */

		return COMPILATION_SUCCESS;
	}
}
