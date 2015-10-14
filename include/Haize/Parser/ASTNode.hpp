#ifndef _HAIZE_ASTNODE_INCLUDE
#define _HAIZE_ASTNODE_INCLUDE

#include <deque>
#include "Haize/Core/Define.hpp"
#include "Haize/Parser/Token.hpp"

namespace hz
{
	namespace parser
	{
		struct ASTNode;
		/*!
		*
		*/
		typedef std::deque<ASTNode*> ASTNodeList;

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
			ASTNodeList* children;
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
