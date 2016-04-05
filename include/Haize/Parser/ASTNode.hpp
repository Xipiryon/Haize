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
			virtual ~ASTNode();

			m::String name;
			hz::parser::eTokenType type;
			std::deque<ASTNode*>* children;
			ASTNode* parent;

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
			m::String declTypename;

			virtual m::String toString();
		};

		struct HAIZE_API ASTNodeArgDecl : ASTNode
		{
			ParamPrefix prefix;
			m::String declTypename;

			virtual m::String toString();
		};

		struct HAIZE_API ASTNodeFunctionDecl : ASTNode
		{
			m::String retTypename;

			virtual m::String toString();
		};

		struct HAIZE_API ASTNodeConstant : ASTNode
		{
			m::Variant value;
			virtual m::String toString();
		};
	}
}

#endif
