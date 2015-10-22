#include <map>
#include <stack>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <Muon/Memory/Allocator.hpp>
#include <Muon/System/Log.hpp>
#include "Haize/Parser/ASTNode.hpp"
#include "Haize/Parser/Syntaxic.hpp"

#define HAIZE_PARSE_PRINT 1
namespace
{
	using namespace muon;

	// ASCII Tree variables
	char g_Depth[2048];
	muon::u32 g_DepthIndex;

	void pushASCII(char c);
	void popASCII();
	void displayASCII(hz::parser::ASTNode* node);

	hz::parser::ASTNode* displayRecursive(std::ostream& graphviz, muon::u32 id, hz::parser::ASTNode* node);

	void initPrecedenceAssoc(hz::parser::Info&);
	bool initParse(hz::parser::Info&);
	muon::u32 getLookUpRule(hz::parser::Info&);

	bool applyShuntingYard(hz::parser::Info&);
	bool createASTfromRPN(hz::parser::Info&);

	enum OpAssociativity
	{
		ASSOC_LEFT,
		ASSOC_RIGHT,
	};

	struct OpAttribute
	{
		muon::u32 precedence;
		OpAssociativity associativity;
	};

	struct NodeOpAttribute
	{
		hz::parser::ASTNode* node;
		OpAttribute attr;
	};

	std::map<hz::parser::eTokenType, OpAttribute> g_OpAttribute;
	std::deque<NodeOpAttribute> g_NodeOpAttribute;

	const muon::u32 INVALID_RULE = 0;
}

namespace hz
{
	namespace parser
	{
		/*
		* Custom parser::InfoImpl
		* Specific for Syntaxic parsing
		*/
		struct InfoImplSyntaxic : InfoImpl
		{
			muon::u32	readIndex;
			ASTNode*	node;

			std::deque<Token> stackOperator;
			std::deque<Token> stackValue;
		};
#define INFO_IMPL ((hz::parser::InfoImplSyntaxic*)info.impl)

		namespace syntaxic
		{
			bool parse(parser::Info& info)
			{
				if (info.ASTRoot)
				{
					while (!info.ASTRoot->children->empty())
					{
						MUON_CDELETE(info.ASTRoot->children->back());
					}
				}
				info.ASTRoot->name = "NT_CHUNK";
				info.ASTRoot->token = Token(NT_CHUNK);

				info.impl = MUON_CNEW(InfoImplSyntaxic);
				INFO_IMPL->node = NULL;
				INFO_IMPL->readIndex = 0;

				bool ret = true;
				if (info.TokenList->empty() || info.TokenList->front().type == S_EOF)
				{
					info.error.message = "Empty token list, nothing to parse.";
					ret = false;
				}
				else
				{
					initPrecedenceAssoc(info);
					ret = initParse(info);
				}

				MUON_CDELETE(INFO_IMPL);
				return ret;
			}

			void display(parser::Info& info)
			{
#ifdef MUON_DEBUG
				muon::system::Log log("SYNTAXIC", muon::LOG_INFO);
				log() << "** Displaying AST in ASCII **" << muon::endl;
				displayASCII(info.ASTRoot);

				log() << "** Outputing AST to \"parse.syntaxic.gv\" file **" << muon::endl;

				std::ofstream graphviz("parse.syntaxic.gv", std::ios::trunc);
				if (graphviz)
				{
					graphviz << "digraph {" << std::endl;
					//graphviz << "graph[splines = ortho]" << std::endl;
					graphviz << "node[shape = record]" << std::endl;
					if (info.ASTRoot)
					{
						displayRecursive(graphviz, 0, info.ASTRoot);
					}
					graphviz << "}" << std::endl;
				}
				graphviz.close();

#	ifdef MUON_PLATFORM_PC
				::system("dot.exe -Tpng parse.syntaxic.gv -o parse.syntaxic.png");
#	else
				::system("dot -Tpng parse.syntaxic.gv -o parse.syntaxic.png");
#	endif
#endif
			}
		}
	}
}

namespace
{
	void pushASCII(char c)
	{
		g_Depth[g_DepthIndex++] = ' ';
		g_Depth[g_DepthIndex++] = c;
		g_Depth[g_DepthIndex++] = ' ';
		g_Depth[g_DepthIndex++] = ' ';
		g_Depth[g_DepthIndex] = 0;
	}

	void popASCII()
	{
		g_Depth[g_DepthIndex -= 4] = 0;
	}

	void displayASCII(hz::parser::ASTNode* node)
	{
		printf("%s \n", hz::parser::TokenTypeStr[node->token.type]);
		for (muon::u32 i = 0; i < node->children->size(); ++i)
		{
			hz::parser::ASTNode* n = (*node->children)[i];
			if (i < node->children->size() - 1)
			{
#ifdef MUON_PLATFORM_WINDOWS
				printf("%s |-", g_Depth);
#else
				printf("%s ├─", g_Depth);
#endif
				pushASCII('|');
			}
			else // Last element
			{
#ifdef MUON_PLATFORM_WINDOWS
				printf("%s `-", g_Depth);
#else
				printf("%s └─", g_Depth);
#endif
				pushASCII(' ');
			}
			displayASCII(n);
			popASCII();
		}
	}

	hz::parser::ASTNode* displayRecursive(std::ostream& graphviz, muon::u32 id, hz::parser::ASTNode* node)
	{
		for (muon::u32 i = 0; i < id; ++i)
			graphviz << "  ";

		graphviz << "\"" << node << "\" [label=\""
			<< hz::parser::TokenTypeStr[node->token.type]
			<< " #" << node->token.line << ":" << node->token.column
			<< "\"]" << std::endl;

		for (auto it = node->children->begin(); it != node->children->end(); ++it)
		{
			hz::parser::ASTNode* cnode = *it;
			displayRecursive(graphviz, id + 1, cnode);
			graphviz << "\"" << node << "\" -> \"" << cnode << "\"" << std::endl;
		}
		return NULL;
	}

	//********************************************************
	// Macro shortcuts

#define TOK(Offset) ((INFO_IMPL->readIndex+Offset) < info.TokenList->size() ? (*info.TokenList)[INFO_IMPL->readIndex+Offset] : hz::parser::Token())
#define TOK_TYPE(Offset) TOK(Offset).type

#define VARIANT(Offset) TOK(Offset).value

#define EXPECT(Offset, Type) TOK_TYPE(Offset) == hz::parser::Type
#define EXPECTK(Offset, Name) ( TOK_TYPE(Offset) == hz::parser::S_KEYWORD ? (*VARIANT(Offset).get<muon::String*>() == Name) : false)

#define CREATENODE(...) MUON_CNEW(hz::parser::ASTNode, __VA_ARGS__)
#define DELETENODE(Node) MUON_CDELETE(Node)
#define CONSUME(Count) consume(info, Count)
#define ERR(Message) info.error.message = Message

	// Functions helper
	void consume(hz::parser::Info& info, muon::u32 count)
	{
#if defined(MUON_DEBUG) && HAIZE_PARSE_PRINT
		muon::system::Log c("Consuming", muon::LOG_DEBUG);
		for (muon::u32 i = 0; i < count; ++i)
		{
			c << hz::parser::TokenTypeStr[TOK_TYPE(0)] << muon::endl;
			INFO_IMPL->readIndex++;
		}
#else
		INFO_IMPL->readIndex += count;
#endif
	}

	//********************************************************

	void initPrecedenceAssoc(hz::parser::Info& info)
	{
		//If Init has already been called, just skip it
		//(Operator precedence won't change at runtime)
		if (!g_OpAttribute.empty())
		{
			return;
		}

		using namespace hz::parser;

		// Precedence value reflect if an operator precedence is
		// higher / lower than another.

		g_OpAttribute[S_LBRACKET] = { 100, ASSOC_LEFT };

		g_OpAttribute[S_RESOLUTION] = { 50, ASSOC_LEFT };
		g_OpAttribute[S_ACCESSOR] = { 30, ASSOC_RIGHT };

		g_OpAttribute[UNARY_PLUS] = { 25, ASSOC_RIGHT };
		g_OpAttribute[UNARY_MINUS] = { 25, ASSOC_RIGHT };
		g_OpAttribute[MATH_INC] = { 25, ASSOC_RIGHT };
		g_OpAttribute[MATH_DEC] = { 25, ASSOC_RIGHT };

		g_OpAttribute[MATH_MUL] = { 20, ASSOC_LEFT };
		g_OpAttribute[MATH_DIV] = { 20, ASSOC_LEFT };
		g_OpAttribute[MATH_MOD] = { 20, ASSOC_LEFT };

		g_OpAttribute[MATH_ADD] = { 15, ASSOC_LEFT };
		g_OpAttribute[MATH_SUB] = { 15, ASSOC_LEFT };

		g_OpAttribute[MATH_ASN] = { 10, ASSOC_RIGHT };
		g_OpAttribute[MATH_ASN_ADD] = { 10, ASSOC_RIGHT };
		g_OpAttribute[MATH_ASN_SUB] = { 10, ASSOC_RIGHT };
		g_OpAttribute[MATH_ASN_MUL] = { 10, ASSOC_RIGHT };
		g_OpAttribute[MATH_ASN_DIV] = { 10, ASSOC_RIGHT };
		g_OpAttribute[MATH_ASN_MOD] = { 10, ASSOC_RIGHT };
		g_OpAttribute[MATH_ASN_AND] = { 10, ASSOC_RIGHT };
		g_OpAttribute[MATH_ASN_OR] = { 10, ASSOC_RIGHT };
		g_OpAttribute[MATH_ASN_XOR] = { 10, ASSOC_RIGHT };
		g_OpAttribute[MATH_ASN_NOT] = { 10, ASSOC_RIGHT };
	}

	void pushNodePrecedence(hz::parser::Info& info, hz::parser::ASTNode* node)
	{
		g_NodeOpAttribute.push_back({ node, g_OpAttribute[node->token.type] });

#if defined(MUON_DEBUG) && HAIZE_PARSE_PRINT
		muon::system::Log log("NodeStack", muon::LOG_DEBUG);
		log() << " + [";
		for (auto it : g_NodeOpAttribute)
		{
			log << hz::parser::TokenTypeStr[it.node->token.type] << ":" << it.attr.precedence << ", ";
		}
		log << "]" << muon::endl;
#endif
	}

	void popNodePrecedence(hz::parser::Info& info)
	{
		MUON_ASSERT(!g_NodeOpAttribute.empty(), "Operator Precedence Stack is empty!");
		if (!g_NodeOpAttribute.empty())
		{
			g_NodeOpAttribute.pop_back();
		}

#if defined(MUON_DEBUG) && HAIZE_PARSE_PRINT
		muon::system::Log log("NodeStack", muon::LOG_DEBUG);
		log() << " - [";
		for (auto it : g_NodeOpAttribute)
		{
			log << hz::parser::TokenTypeStr[it.node->token.type] << ":" << it.attr.precedence << ", ";
		}
		log << "]" << muon::endl;
#endif
	}

	bool checkForLowerPrecOrDiffAssoc(hz::parser::Info& info, hz::parser::ASTNode* node)
	{
		const OpAttribute& op = g_OpAttribute[node->token.type];
		const OpAttribute& opStack = g_NodeOpAttribute.back().attr;

		return ((op.precedence < opStack.precedence)
				|| (op.associativity == ASSOC_LEFT && opStack.associativity == ASSOC_RIGHT));
	}

	muon::u32 getLookUpRule(hz::parser::Info& info)
	{
		/*
		muon::u32 nextIndex = INFO_IMPL->readIndex + 1;
		if (nextIndex >= info.TokenList->size())
		{
			MUON_ERROR("Trying to get next rule while there is no next Token!");
			return INVALID_RULE;
		}

		hz::parser::eTokenType currType = (*info.TokenList)[INFO_IMPL->readIndex].type;
		hz::parser::eTokenType currCategory = (*info.TokenList)[INFO_IMPL->readIndex].category;
		hz::parser::eTokenType nextType = (*info.TokenList)[nextIndex].type;
		hz::parser::eTokenType nextCategory = (*info.TokenList)[nextIndex].category;

#define HAIZE_RULE(Curr, Next, lToken, rToken, Rule) if( (Curr == lToken) && (Next == rToken)) { return Rule; }
		using namespace hz::parser;

		// IDENTIFIER
		HAIZE_RULE(currType, nextCategory, V_IDENTIFIER, CATEGORY_BINOP, 4);
		HAIZE_RULE(currType, nextCategory, V_IDENTIFIER, S_EOF, 26);

		// CONSTANTS
		HAIZE_RULE(currCategory, nextCategory, CATEGORY_CONSTANT, CATEGORY_BINOP, 4);
		HAIZE_RULE(currCategory, nextCategory, CATEGORY_CONSTANT, S_EOF, 26);

		// UNOP

		// BINOP
		HAIZE_RULE(currCategory, nextType, CATEGORY_BINOP, V_IDENTIFIER, 88);
		HAIZE_RULE(currCategory, nextCategory, CATEGORY_BINOP, CATEGORY_CONSTANT, 89);
		HAIZE_RULE(currCategory, nextType, CATEGORY_BINOP, CATEGORY_UNOP, 90);
		HAIZE_RULE(currCategory, nextType, CATEGORY_BINOP, S_LPARENT, 92);

		// (

		// )

		// [

		// ]

		// {
		// }

		// ,

		// ::

		// SEPARATOR (; \n)

		// if

		// else

		// for

		// in

		// while

		// switch

		// case

		// default

		// break

		// continue

		// namespace

		// class

		// function

		// attr

		// global

		// EOF
		// */
		return INVALID_RULE;
#undef HAIZE_RULE
	}

	bool initParse(hz::parser::Info& info)
	{
		muon::system::Log log("Syntaxic");
		bool parse = true;
		while (parse)
		{
			bool useShuntingYard = true;

			// Create AST for expression with Shunting Yard Algorithm
			// "a+b" -> "a b +" ->    +
			//                       / \
			//                      a   b
			if(useShuntingYard)
			{
				parse = applyShuntingYard(info);
			}
			else
			{
				CONSUME(1);
			}
			parse &= (TOK_TYPE(0) != hz::parser::S_EOF);
		}
		return true;
	}

	bool applyShuntingYard(hz::parser::Info& info)
	{
		bool ret = true;
		// While there are tokens to be read:
		// 	Read a token.
		// 	If the token is a number, then add it to the output queue.
		// 	If the token is a function token, then push it onto the stack.
		// 	If the token is a function argument separator (e.g., a comma):
		// 		Until the token at the top of the stack is a left parenthesis, pop operators off the stack onto the output queue. If no left parentheses are encountered, either the separator was misplaced or parentheses were mismatched.
		// 	If the token is an operator, o1, then:
		// 		while there is an operator token, o2, at the top of the operator stack, and either
		// 			o1 is left-associative and its precedence is less than or equal to that of o2, or
		// 			o1 is right associative, and has precedence less than that of o2,
		// 			then pop o2 off the operator stack, onto the output queue;
		// 		push o1 onto the operator stack.
		// 	If the token is a left parenthesis (i.e. "("), then push it onto the stack.
		// 	If the token is a right parenthesis (i.e. ")"):
		// 		Until the token at the top of the stack is a left parenthesis, pop operators off the stack onto the output queue.
		// 		Pop the left parenthesis from the stack, but not onto the output queue.
		// 		If the token at the top of the stack is a function token, pop it onto the output queue.
		// 		If the stack runs out without finding a left parenthesis, then there are mismatched parentheses.
		// 	When there are no more tokens to read:
		// 		While there are still operator tokens in the stack:
		// 		If the operator token on the top of the stack is a parenthesis, then there are mismatched parentheses.
		// 		Pop the operator onto the output queue.
		// Exit.
		hz::parser::Token currToken;
		currToken = TOK(0);

		//TODO: Parenthesis : ( expr )
		//TODO: Function call : foo ( args )
		//TODO: Real end condition for SY algorithm
		//TODO: Error
		while(currToken.type != hz::parser::S_EOF && currToken.type != hz::parser::S_INVALID)
		{
			if(currToken.category == hz::parser::CATEGORY_CONSTANT
				|| currToken.type == hz::parser::V_IDENTIFIER)
			{
				INFO_IMPL->stackValue.push_back(currToken);
			}
			else if (currToken.category == hz::parser::CATEGORY_BINOP)
			{
				const hz::parser::Token& op1 = currToken; //Just o1 & o2 syntax
				hz::parser::Token op2;
				bool popOp = !INFO_IMPL->stackOperator.empty();
				while(popOp)
				{
					op2 = INFO_IMPL->stackOperator.back();
					if( (g_OpAttribute[op1.type].associativity == ASSOC_LEFT && g_OpAttribute[op1.type].precedence <= g_OpAttribute[op2.type].precedence)
						 || (g_OpAttribute[op1.type].associativity == ASSOC_RIGHT && g_OpAttribute[op1.type].precedence < g_OpAttribute[op2.type].precedence))
					{
						INFO_IMPL->stackValue.push_back(op2);
						INFO_IMPL->stackOperator.pop_back();
						popOp = !INFO_IMPL->stackOperator.empty();
					}
					else
					{
						popOp = false;
					}
				}
				INFO_IMPL->stackOperator.push_back(op1);
			}

			CONSUME(1);
			currToken = TOK(0);
		}
		// Empty operator queue
		while(!INFO_IMPL->stackOperator.empty())
		{
			hz::parser::Token op(INFO_IMPL->stackOperator.back());
			INFO_IMPL->stackValue.push_back(op);
			INFO_IMPL->stackOperator.pop_back();
		}

		ret = createASTfromRPN(info);
		return ret;
	}

	bool createASTfromRPN(hz::parser::Info& info)
	{
		bool ret = true;

		return ret;
	}
}