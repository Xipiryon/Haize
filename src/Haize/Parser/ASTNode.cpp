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

		ASTNode::~ASTNode()
		{
			for (auto it = children->begin(); it != children->end(); ++it)
			{
				delete *it;
			}
			delete children;
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

		m::String ASTNode::toString()
		{
			return hz::parser::TokenTypeStr[type];
		}

		m::String ASTNodeNamespaceDecl::toString()
		{
			return "namespace " + name;
		}

		m::String ASTNodeVarDecl::toString()
		{
			return (global ? "(global) " : "") + declTypename + " " + name;
		}

		m::String ASTNodeArgDecl::toString()
		{
			return ((prefix & IN) ? "IN" : "") + m::String((prefix & OUT) ? "OUT" : "") + " " + declTypename + " " + name;
		}

		m::String ASTNodeFunctionDecl::toString()
		{
			return retTypename + " " + name;
		}

		m::String ASTNodeConstant::toString()
		{
			char buffer[32];
			m::String str;
			m::meta::MetaData* m = value.getMeta();
			if (MUON_META(m::String) == m)
			{
				str = value.get<m::String>();
			}
			else if (MUON_META(m::f32) == m)
			{
				m::ftoa(value.get<m::f32>(), buffer);
				str = buffer;
			}
			else if (MUON_META(m::i32) == m)
			{
				m::itoa((m::i64)value.get<m::i32>(), buffer);
				str = buffer;
			}
			else if (MUON_META(bool) == m)
			{
				str = (value.get<bool>() ? "true" : "false");
			}
			else
			{
				str = TokenTypeStr[type];
			}
			return str;
		}
	}
}
