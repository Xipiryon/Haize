
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
		muon::u32 precedence;
		OpAssociativity associativity;
	};

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
		std::map<hz::parser::eTokenType, OpAttribute> precAttrib;
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

		if (m_tokenList->empty() || m_tokenList->front().type == parser::S_EOF)
		{
			m_error.message = "Empty token list, nothing to parse.";
			return COMPILATION_ERROR_SYNTAXIC;
		}

		//initPrecedenceAssoc(&info);
		return COMPILATION_ERROR_SYNTAXIC;
	}

	void Context::initPrecedenceAssoc(parser::InfoImpl* info)
	{
		InfoSyntaxic* impl = (InfoSyntaxic*)info;

		// Precedence value reflect if an operator precedence is
		// higher / lower than another.

		muon::u32 precedence = 100;
		impl->precAttrib[parser::S_LBRACKET] = { precedence, ASSOC_LEFT };

		--precedence;
		impl->precAttrib[parser::S_RESOLUTION] = { precedence, ASSOC_LEFT };

		--precedence;
		impl->precAttrib[parser::S_ACCESSOR] = { precedence, ASSOC_RIGHT };

		--precedence;
		impl->precAttrib[parser::UNARY_PLUS] = { precedence, ASSOC_RIGHT };
		impl->precAttrib[parser::UNARY_MINUS] = { precedence, ASSOC_RIGHT };
		impl->precAttrib[parser::MATH_INC] = { precedence, ASSOC_RIGHT };
		impl->precAttrib[parser::MATH_DEC] = { precedence, ASSOC_RIGHT };
		impl->precAttrib[parser::LOGIC_NOT] = { precedence, ASSOC_RIGHT };
		impl->precAttrib[parser::BITWISE_NOT] = { precedence, ASSOC_RIGHT };

		--precedence;
		impl->precAttrib[parser::MATH_MUL] = { precedence, ASSOC_LEFT };
		impl->precAttrib[parser::MATH_DIV] = { precedence, ASSOC_LEFT };
		impl->precAttrib[parser::MATH_MOD] = { precedence, ASSOC_LEFT };

		--precedence;
		impl->precAttrib[parser::MATH_ADD] = { precedence, ASSOC_LEFT };
		impl->precAttrib[parser::MATH_SUB] = { precedence, ASSOC_LEFT };

		--precedence;
		impl->precAttrib[parser::BITWISE_LSH] = { precedence, ASSOC_LEFT };
		impl->precAttrib[parser::BITWISE_RSH] = { precedence, ASSOC_LEFT };

		--precedence;
		impl->precAttrib[parser::LOGIC_GT] = { precedence, ASSOC_LEFT };
		impl->precAttrib[parser::LOGIC_GET] = { precedence, ASSOC_LEFT };
		impl->precAttrib[parser::LOGIC_LT] = { precedence, ASSOC_LEFT };
		impl->precAttrib[parser::LOGIC_LET] = { precedence, ASSOC_LEFT };

		--precedence;
		impl->precAttrib[parser::LOGIC_EQ] = { precedence, ASSOC_LEFT };
		impl->precAttrib[parser::LOGIC_NEQ] = { precedence, ASSOC_LEFT };

		--precedence;
		impl->precAttrib[parser::BITWISE_AND] = { precedence, ASSOC_LEFT };

		--precedence;
		impl->precAttrib[parser::BITWISE_XOR] = { precedence, ASSOC_LEFT };

		--precedence;
		impl->precAttrib[parser::BITWISE_OR] = { precedence, ASSOC_LEFT };

		--precedence;
		impl->precAttrib[parser::LOGIC_AND] = { precedence, ASSOC_LEFT };

		--precedence;
		impl->precAttrib[parser::LOGIC_OR] = { precedence, ASSOC_LEFT };

		--precedence;
		impl->precAttrib[parser::MATH_ASN] = { precedence, ASSOC_RIGHT };
		impl->precAttrib[parser::MATH_ASN_ADD] = { precedence, ASSOC_RIGHT };
		impl->precAttrib[parser::MATH_ASN_SUB] = { precedence, ASSOC_RIGHT };
		impl->precAttrib[parser::MATH_ASN_MUL] = { precedence, ASSOC_RIGHT };
		impl->precAttrib[parser::MATH_ASN_DIV] = { precedence, ASSOC_RIGHT };
		impl->precAttrib[parser::MATH_ASN_MOD] = { precedence, ASSOC_RIGHT };
		impl->precAttrib[parser::MATH_ASN_AND] = { precedence, ASSOC_RIGHT };
		impl->precAttrib[parser::MATH_ASN_OR] = { precedence, ASSOC_RIGHT };
		impl->precAttrib[parser::MATH_ASN_XOR] = { precedence, ASSOC_RIGHT };
		impl->precAttrib[parser::MATH_ASN_NOT] = { precedence, ASSOC_RIGHT };
	}
}
