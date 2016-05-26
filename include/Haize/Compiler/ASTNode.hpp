#ifndef INCLUDE_HAIZE_ASTNODE_INCLUDED
#define INCLUDE_HAIZE_ASTNODE_INCLUDED

#include <deque>
#include <Muon/Variant.hpp>

#include "Haize/Compiler/Token.hpp"
#include "Haize/Compiler/DeclStruct.hpp"

namespace hz
{
	namespace parser
	{
		enum eParamPrefix
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
			DeclNamespace decl;

			virtual m::String toString();
		};

		struct HAIZE_API ASTNodeMemberDecl : ASTNode
		{
			DeclVariable decl;

			virtual m::String toString();
		};

		struct HAIZE_API ASTNodeVariableManipulation : ASTNode
		{
			DeclVariable decl;

			virtual m::String toString();
		};

		struct HAIZE_API ASTNodeArgDecl : ASTNode
		{
			DeclVariable decl;

			virtual m::String toString();
		};

		struct HAIZE_API ASTNodeFunctionDecl : ASTNode
		{
			DeclFunction decl;

			virtual m::String toString();
		};

		struct HAIZE_API ASTNodeClassDecl : ASTNode
		{
			DeclClass decl;

			virtual m::String toString();
		};

		struct HAIZE_API ASTNodeFunctionCall : ASTNode
		{
			DeclFunction decl;

			virtual m::String toString();
		};

		struct HAIZE_API ASTNodeConstant : ASTNode
		{
			DeclVariable decl;

			virtual m::String toString();
		};

		struct HAIZE_API ASTNodeParenthesis : ASTNode
		{
			ASTNode* funcIdentNode;
			m::u32 exprValueIndex;
		};

		struct HAIZE_API ASTNodeOperator : ASTNode
		{
			bool binop;
		};
	}
}

#endif
