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

		struct IRInfo
		{
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
			virtual IRInfo generateCode();
		};

		struct HAIZE_API ASTNodeNamespaceDecl : ASTNode
		{
			virtual m::String toString();
			virtual IRInfo generateCode();
		};

		struct HAIZE_API ASTNodeMemberDecl : ASTNode
		{
			m::String declTypename;

			virtual m::String toString();
			virtual IRInfo generateCode();
		};

		struct HAIZE_API ASTNodeVarNew : ASTNode
		{
			bool global;
			m::String declTypename;

			virtual m::String toString();
			virtual IRInfo generateCode();
		};

		struct HAIZE_API ASTNodeVarDelete : ASTNode
		{
			virtual m::String toString();
			virtual IRInfo generateCode();
		};

		struct HAIZE_API ASTNodeArgDecl : ASTNode
		{
			ParamPrefix prefix;
			m::String declTypename;

			virtual m::String toString();
			virtual IRInfo generateCode();
		};

		struct HAIZE_API ASTNodeFunctionDecl : ASTNode
		{
			m::String retTypename;

			virtual m::String toString();
			virtual IRInfo generateCode();
		};

		struct HAIZE_API ASTNodeClassDecl : ASTNode
		{
			virtual m::String toString();
			virtual IRInfo generateCode();
		};

		struct HAIZE_API ASTNodeFunctionCall : ASTNode
		{
			virtual IRInfo generateCode();
		};

		struct HAIZE_API ASTNodeArgCall : ASTNode
		{
			virtual IRInfo generateCode();
		};

		struct HAIZE_API ASTNodeConstant : ASTNode
		{
			m::Variant value;

			virtual m::String toString();
			virtual IRInfo generateCode();
		};

		struct HAIZE_API ASTNodeParenthesis : ASTNode
		{
			ASTNode* funcIdentNode;
			m::u32 exprValueIndex;

			virtual IRInfo generateCode();
		};

		struct HAIZE_API ASTNodeOperator : ASTNode
		{
			bool binop;

			virtual IRInfo generateCode();
		};
	}
}

#endif
