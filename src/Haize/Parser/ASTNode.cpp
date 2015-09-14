
#include <Muon/Memory/Allocator.hpp>
#include <Muon/System/Log.hpp>

#include "Haize/Parser/ASTNode.hpp"

namespace hz
{
	namespace parser
	{
		ASTNode::ASTNode()
			: name(NULL)
			, token(S_INVALID)
			, children(MUON_CNEW(ASTNodeList))
			, parent(NULL)
		{
		}

		ASTNode::ASTNode(eTokenType type_, const char* name_)
			: name(name_)
			, token(type_)
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

		ASTNode* ASTNode::addChild(eTokenType type, const char* name)
		{
			ASTNode* node = MUON_CNEW(ASTNode, type, name);
			node->parent = this;
			this->children->push_back(node);
			return node;
		}

		ASTNode* ASTNode::addChild(const Token& token)
		{
			ASTNode* c = addChild(token.type, TokenTypeStr[token.type]);
			this->token = token;
			return c;
		}

		ASTNode* ASTNode::addChild(ASTNode* child)
		{
			child->parent = this;
			this->children->push_back(child);
			return child;
		}

		bool ASTNode::removeChild(ASTNode* child)
		{
			for (u32 i = 0; i < this->children->size(); ++i)
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
			if(removeChild(child))
			{
				MUON_CDELETE(child);
				return true;
			}
			return false;
		}
	}
}
