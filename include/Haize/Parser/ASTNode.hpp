#ifndef _HAIZE_ASTNODE_INCLUDE
#define _HAIZE_ASTNODE_INCLUDE

#include <deque>
#include <Muon/String.hpp>
#include <Muon/Variant.hpp>
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
			ASTNode(muon::u32, const char*);
			~ASTNode();

			const char* name;
			muon::u32 type;
			ASTNodeList* children;
			ASTNode* parent;
			muon::Variant value;

			muon::String section;
			muon::String function;
			muon::u32 line;
			muon::u32 column;


			ASTNode* addChild(muon::u32, const char* = NULL);
			ASTNode* addChild(ASTNode*);
			bool removeChild(ASTNode*);
			bool deleteChild(ASTNode*);
		};
	}
}

#endif
