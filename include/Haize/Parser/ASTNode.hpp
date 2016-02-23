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
			ASTNode(eTokenType, const char*);
			ASTNode(const Token&);
			~ASTNode();

			const char* name;
			Token token;
			std::deque<ASTNode*>* children;
			ASTNode* parent;

			ASTNode* addChild(eTokenType, const char*);
			ASTNode* addChild(const Token&);
			ASTNode* addChild(ASTNode*);
			bool removeChild(ASTNode*);
			bool deleteChild(ASTNode*);
		};
	}
}

#endif
