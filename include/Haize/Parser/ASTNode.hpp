#ifndef INCLUDE_HAIZE_ASTNODE_INCLUDED
#define INCLUDE_HAIZE_ASTNODE_INCLUDED

#include <deque>
#include <Muon/Variant.hpp>

#include "Haize/Parser/Token.hpp"

namespace hz
{
	namespace parser
	{
		enum ParamPrefix
		{
			IN = 0x1,
			OUT = 0x2,
		};

		/*!
		*
		*/
		struct HAIZE_API ASTNode
		{
			ASTNode();
			ASTNode(m::u32);
			ASTNode(m::u32, const m::String&);
			ASTNode(const Token&);
			~ASTNode();

			m::String name;
			m::u32 type;
			m::Variant value;
			std::deque<ASTNode*>* children;
			ASTNode* parent;
			Token token;

			ASTNode* addChild(m::u32);
			ASTNode* addChild(m::u32, const m::String&);
			ASTNode* addChild(ASTNode*);
			bool removeChild(ASTNode*);
			bool deleteChild(ASTNode*);
		};

		struct HAIZE_API ASTNodeVarDecl : ASTNode
		{
			m::String typeName;
			m::String varName;
		};
	}
}

#endif
