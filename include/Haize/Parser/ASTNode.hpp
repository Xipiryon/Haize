#ifndef _HAIZE_ASTNODE_INCLUDE
#define _HAIZE_ASTNODE_INCLUDE

#include <deque>
#include <Muon/Meta/Variant.hpp>
#include "Haize/Core/Define.hpp"

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
			ASTNode(u32, const char* = NULL);
			~ASTNode();

			const char* name;
			u32 type;
			meta::Variant value;
			ASTNodeList* children;
			ASTNode* parent;

			ASTNode* addChild(u32, const char* = NULL);
			void addChild(ASTNode*);
			void removeChild(ASTNode*);
		};
	}
}

#endif
