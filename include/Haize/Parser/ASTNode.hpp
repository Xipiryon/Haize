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
			~ASTNode();

			m::String name;
			m::u32 type;
			std::deque<ASTNode*>* children;
			ASTNode* parent;

			//m::Variant value;
			//Token token;

			ASTNode* addChild(ASTNode*);
			bool removeChild(ASTNode*);
			bool deleteChild(ASTNode*);

			virtual m::String toString();
		};

		struct HAIZE_API ASTNodeNamespaceDecl : ASTNode
		{
			virtual m::String toString();
		};

		struct HAIZE_API ASTNodeVarDecl : ASTNode
		{
			bool global;
			m::String typeName;
			m::String varName;

			virtual m::String toString();
		};
	}
}

#endif
