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

	std::map<hz::parser::eTokenType, OpAttribute> g_OpAttribute;

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
			std::deque<ASTNode*> stackValue;
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
#define TOK_DEBUG(T) do { muon::system::Log l("*Token*", muon::LOG_DEBUG); l() << hz::parser::TokenTypeStr[T.type] << " | " <<hz::parser::TokenTypeStr[T.category] << muon::endl; } while(false);

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

		u32 precedence = 100;
		g_OpAttribute[S_LBRACKET] = { precedence, ASSOC_LEFT };

		--precedence;
		g_OpAttribute[S_RESOLUTION] = { precedence, ASSOC_LEFT };

		--precedence;
		g_OpAttribute[S_ACCESSOR] = { precedence, ASSOC_RIGHT };

		--precedence;
		g_OpAttribute[UNARY_PLUS] = { precedence, ASSOC_RIGHT };
		g_OpAttribute[UNARY_MINUS] = { precedence, ASSOC_RIGHT };
		g_OpAttribute[MATH_INC] = { precedence, ASSOC_RIGHT };
		g_OpAttribute[MATH_DEC] = { precedence, ASSOC_RIGHT };
		g_OpAttribute[LOGIC_NOT] = { precedence, ASSOC_RIGHT };
		g_OpAttribute[BITWISE_NOT] = { precedence, ASSOC_RIGHT };

		--precedence;
		g_OpAttribute[MATH_MUL] = { precedence, ASSOC_LEFT };
		g_OpAttribute[MATH_DIV] = { precedence, ASSOC_LEFT };
		g_OpAttribute[MATH_MOD] = { precedence, ASSOC_LEFT };

		--precedence;
		g_OpAttribute[MATH_ADD] = { precedence, ASSOC_LEFT };
		g_OpAttribute[MATH_SUB] = { precedence, ASSOC_LEFT };

		--precedence;
		g_OpAttribute[BITWISE_LSH] = { precedence, ASSOC_LEFT };
		g_OpAttribute[BITWISE_RSH] = { precedence, ASSOC_LEFT };

		--precedence;
		g_OpAttribute[LOGIC_GT] = { precedence, ASSOC_LEFT };
		g_OpAttribute[LOGIC_GET] = { precedence, ASSOC_LEFT };
		g_OpAttribute[LOGIC_LT] = { precedence, ASSOC_LEFT };
		g_OpAttribute[LOGIC_LET] = { precedence, ASSOC_LEFT };

		--precedence;
		g_OpAttribute[LOGIC_EQ] = { precedence, ASSOC_LEFT };
		g_OpAttribute[LOGIC_NEQ] = { precedence, ASSOC_LEFT };

		--precedence;
		g_OpAttribute[BITWISE_AND] = { precedence, ASSOC_LEFT };

		--precedence;
		g_OpAttribute[BITWISE_XOR] = { precedence, ASSOC_LEFT };

		--precedence;
		g_OpAttribute[BITWISE_OR] = { precedence, ASSOC_LEFT };

		--precedence;
		g_OpAttribute[LOGIC_AND] = { precedence, ASSOC_LEFT };

		--precedence;
		g_OpAttribute[LOGIC_OR] = { precedence, ASSOC_LEFT };

		--precedence;
		g_OpAttribute[MATH_ASN] = { precedence, ASSOC_RIGHT };
		g_OpAttribute[MATH_ASN_ADD] = { precedence, ASSOC_RIGHT };
		g_OpAttribute[MATH_ASN_SUB] = { precedence, ASSOC_RIGHT };
		g_OpAttribute[MATH_ASN_MUL] = { precedence, ASSOC_RIGHT };
		g_OpAttribute[MATH_ASN_DIV] = { precedence, ASSOC_RIGHT };
		g_OpAttribute[MATH_ASN_MOD] = { precedence, ASSOC_RIGHT };
		g_OpAttribute[MATH_ASN_AND] = { precedence, ASSOC_RIGHT };
		g_OpAttribute[MATH_ASN_OR] = { precedence, ASSOC_RIGHT };
		g_OpAttribute[MATH_ASN_XOR] = { precedence, ASSOC_RIGHT };
		g_OpAttribute[MATH_ASN_NOT] = { precedence, ASSOC_RIGHT };
	}

	muon::u32 getLookUpRule(hz::parser::Info& info)
	{
		return INVALID_RULE;
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
				if(parse)
				{
					parse = createASTfromRPN(info);
				}
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
		// Inspired by https://en.wikipedia.org/wiki/Shunting-yard_algorithm#The_algorithm_in_detail

		hz::parser::Token currToken;
		currToken = TOK(0);
		MUON_ASSERT_BREAK(INFO_IMPL->stackOperator.empty(), "Operator Stack is not empty!");
		MUON_ASSERT_BREAK(INFO_IMPL->stackValue.empty(), "Value Stack is not empty!");

		//TODO: Function call : foo ( args )
		while(currToken.type != hz::parser::S_EOF && currToken.type != hz::parser::S_INVALID)
		{
			// ** Value **
			if(currToken.category == hz::parser::CATEGORY_CONSTANT
				|| currToken.type == hz::parser::V_IDENTIFIER)
			{
				INFO_IMPL->stackValue.push_back(MUON_CNEW(hz::parser::ASTNode, currToken));
			}
			// ** Operator **
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
						INFO_IMPL->stackValue.push_back(MUON_CNEW(hz::parser::ASTNode, op2));
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
			// ** Left Parenthesis **
			else if (currToken.type == hz::parser::S_LPARENT)
			{
				INFO_IMPL->stackOperator.push_back(currToken);
			}
			// ** Right Parenthesis **
			else if (currToken.type == hz::parser::S_RPARENT)
			{
				if(INFO_IMPL->stackOperator.empty())
				{
					ERR("Missing '('");
					return false;
				}
				hz::parser::Token op = INFO_IMPL->stackOperator.back();
				while(op.type != hz::parser::S_LPARENT)
				{
					INFO_IMPL->stackValue.push_back(MUON_CNEW(hz::parser::ASTNode, op));
					INFO_IMPL->stackOperator.pop_back();
					if(INFO_IMPL->stackOperator.empty())
					{
						ERR("Missing '('");
						return false;
					}
					op = INFO_IMPL->stackOperator.back();
				}
				INFO_IMPL->stackOperator.pop_back(); // Pop left parenthesis
			}

			CONSUME(1);
			currToken = TOK(0);

			//And of expression
			if(currToken.type == hz::parser::S_SEPARATOR)
			{
				CONSUME(1);
				break;
			}
		}

		// Empty operator queue
		while(!INFO_IMPL->stackOperator.empty())
		{
			hz::parser::Token op(INFO_IMPL->stackOperator.back());
			INFO_IMPL->stackValue.push_back(MUON_CNEW(hz::parser::ASTNode, op));
			INFO_IMPL->stackOperator.pop_back();
		}

		return ret;
	}

	bool createASTfromRPN(hz::parser::Info& info)
	{
		bool ret = true;
		muon::u32 i = 0;

		hz::parser::ASTNode* node = NULL;
		while(i < INFO_IMPL->stackValue.size())
		{
			hz::parser::Token op = INFO_IMPL->stackValue[i]->token;
			if(op.category == hz::parser::CATEGORY_BINOP)
			{
				// If we've less than 2 variable on the left, there is a problem
				if(i < 2)
				{
					return false;
				}
				// Else, pop them, and add them as children
				node = INFO_IMPL->stackValue[i];
				node->addChild(INFO_IMPL->stackValue[i-2]);
				node->addChild(INFO_IMPL->stackValue[i-1]);
				INFO_IMPL->stackValue.erase(INFO_IMPL->stackValue.begin()+(--i)); // erase right
				INFO_IMPL->stackValue.erase(INFO_IMPL->stackValue.begin()+(--i)); // erase left
			}
			++i;
		}
		//TODO: check there is only one operator left ?
		info.ASTRoot->addChild(INFO_IMPL->stackValue.back());
		INFO_IMPL->stackValue.pop_back();
		return ret;
	}
}
