#ifndef INCLUDE_HAIZE_ASTNODE_INCLUDED
#define INCLUDE_HAIZE_ASTNODE_INCLUDED

#include <deque>
#include "Haize/Parser/Token.hpp"

namespace hz
{
	namespace parser
	{
		/*!
		*
		*/
		struct HAIZE_API ASTNode
		{
			ASTNode();
			ASTNode(eTokenType, const m::String&);
			ASTNode(const Token&);
			~ASTNode();

			m::String name;
			Token token;
			std::deque<ASTNode*>* children;
			ASTNode* parent;

			ASTNode* addChild(eTokenType, const m::String&);
			ASTNode* addChild(const Token&);
			ASTNode* addChild(ASTNode*);
			bool removeChild(ASTNode*);
			bool deleteChild(ASTNode*);
		};
	}
}

#endif
