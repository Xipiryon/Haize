#include <Muon/Memory/Allocator.hpp>
#include <Muon/System/Log.hpp>

#include "Haize/Parser/ASTNode.hpp"

namespace hz
{
	namespace parser
	{
		ASTNode::ASTNode()
			: name("#?#")
			, children(MUON_NEW(std::deque<ASTNode*>))
			, parent(NULL)
		{
		}

		ASTNode::ASTNode(m::u32 type_)
			: name("#?#")
			, children(MUON_NEW(std::deque<ASTNode*>))
			, parent(NULL)
		{
			type = type_;
		}

		ASTNode::ASTNode(m::u32 type_, const m::String& name_)
			: name(name_)
			, children(MUON_NEW(std::deque<ASTNode*>))
			, parent(NULL)
		{
			type = type_;
		}

		ASTNode::~ASTNode()
		{
			for (auto it = children->begin(); it != children->end(); ++it)
			{
				delete *it;
			}
			delete children;
		}

		ASTNode* ASTNode::addChild(m::u32 type)
		{
			return addChild(type, "#?#");
		}

		ASTNode* ASTNode::addChild(m::u32 type, const m::String& name)
		{
			ASTNode* node = MUON_NEW(ASTNode, type, name);
			node->parent = this;
			this->children->push_back(node);
			return node;
		}

		ASTNode* ASTNode::addChild(ASTNode* child)
		{
			child->parent = this;
			this->children->push_back(child);
			return child;
		}

		bool ASTNode::removeChild(ASTNode* child)
		{
			for (m::u32 i = 0; i < this->children->size(); ++i)
			{
				if (this->children->at(i) == child)
				{
					child->parent = NULL;
					this->children->erase(this->children->begin() + i);
					return true;
				}
			}
			return false;
		}

		bool ASTNode::deleteChild(ASTNode* child)
		{
			if (removeChild(child))
			{
				MUON_DELETE(child);
				return true;
			}
			return false;
		}
	}
}
