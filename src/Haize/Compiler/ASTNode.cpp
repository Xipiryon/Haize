#include <Muon/Memory/Utils.hpp>
#include <Muon/System/Log.hpp>

#include "Haize/Compiler/ASTNode.hpp"

namespace hz
{
	namespace parser
	{
		ASTNode::ASTNode()
			: name("")
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
			return name;
		}

		m::String ASTNodeNamespaceDecl::toString()
		{
			return decl.name;
		}

		m::String ASTNodeMemberDecl::toString()
		{
			return decl.type + decl.name;
		}

		m::String ASTNodeVariableManipulation::toString()
		{
			return (type == NT_EXPR_NEW ?
					(decl.global ? "(GLOBAL) " : "") + decl.type + " " + decl.name
					: decl.name);
		}

		m::String ASTNodeArgDecl::toString()
		{
			m::String str;
			switch (decl.prefix)
			{
				case IN: str = "(IN) "; break;
				case OUT: str = "(OUT) "; break;
				case IN | OUT: str = "(INOUT) "; break;
				default: str = "(???) "; break;
			}
			return str + decl.type + " " + name;
		}

		m::String ASTNodeClassDecl::toString()
		{
			return decl.name;
		}

		m::String ASTNodeFunctionDecl::toString()
		{
			m::String str = decl.type + " " + decl.name + " (";
			if (!decl.params.empty())
			{
				for (m::u32 i = 0; i < decl.params.size() - 1; ++i)
				{
					str += decl.params[i].type + ", ";
				}
				str += decl.params.back().type;
			}
			str += ")";
			return str;
		}

		m::String ASTNodeFunctionCall::toString()
		{
			m::String str = decl.name + " (";
			if (!decl.params.empty())
			{
				for (m::u32 i = 0; i < decl.params.size() - 1; ++i)
				{
					str += decl.params[i].type + ", ";
				}
				str += decl.params.back().type;
			}
			str += ")";
			return str;
		}

		m::String ASTNodeConstant::toString()
		{
			char buffer[32];
			m::String str;
			m::u64 id = decl.value.id();
			if (MUON_TRAITS_GET_ID(m::String) == id)
			{
				str = decl.value.get<m::String>();
			}
			else if (MUON_TRAITS_GET_ID(m::f32) == id)
			{
				m::ftoa(decl.value.get<m::f32>(), buffer);
				str = buffer;
			}
			else if (MUON_TRAITS_GET_ID(m::i32) == id)
			{
				m::itoa((m::i64)decl.value.get<m::i32>(), buffer);
				str = buffer;
			}
			else if (MUON_TRAITS_GET_ID(bool) == id)
			{
				str = (decl.value.get<bool>() ? "true" : "false");
			}
			else
			{
				str = TokenTypeStr[type];
			}
			return str;
		}
	}
}
