
#include <Muon/Memory/Allocator.hpp>
#include <Muon/System/Log.hpp>

#include "Haize/Parser/ASTNode.hpp"

namespace hz
{
	namespace parser
	{
		ASTNode::ASTNode()
			: name(NULL)
			, type(0)
			, children(MUON_CNEW(ASTNodeList))
			, parent(NULL)
		{
		}

		ASTNode::ASTNode(u32 type_, const char* name_)
			: name(name_)
			, type(type_)
			, children(MUON_CNEW(ASTNodeList))
			, parent(NULL)
		{
		}

		ASTNode::~ASTNode()
		{
			for (auto it = children->begin(); it != children->end(); ++it)
			{
				delete *it;
			}
			delete children;
		}

		ASTNode* ASTNode::addChild(u32 type, const char* name)
		{
			ASTNode* node = MUON_CNEW(ASTNode, type, name);
			node->parent = this;
			this->children->push_back(node);
			return node;
		}

		void ASTNode::addChild(ASTNode* child)
		{
			child->parent = this;
			this->children->push_back(child);
		}

		void ASTNode::removeChild(ASTNode* child)
		{
			for (u32 i = 0; i < this->children->size(); ++i)
			{
				if (this->children->at(i) == child)
				{
					child->parent = NULL;
					this->children->erase(this->children->begin() + i);
					break;
				}
			}
		}
	}
}
