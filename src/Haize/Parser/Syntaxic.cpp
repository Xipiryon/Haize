
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
	void initLookupTable(hz::parser::Info&);
	bool initParse(hz::parser::Info&);

	enum ParserStep
	{
		STMT_DECL = 0,
		FUNC_DECL,
		CLASS_DECL,

		PARSESTEP_COUNT
	};

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

	const muon::u32 LOOKUP_ERROR_INDEX = 0;
	const muon::u32 LOOKUP_ELEMENT_COUNT = 256;
	//TODO: Set the exact number of maximum combinaison
	muon::u32 g_LookUpTable[PARSESTEP_COUNT][LOOKUP_ELEMENT_COUNT];
}

namespace hz
{
	namespace parser
	{
		/*
		* Custom parser::InfoImpl
		* Specific for Syntaxic parsing
		*/
		enum ParserState
		{
			STATE_RUNNING,
			STATE_SKIPPING,
			STATE_DONE
		};
		struct InfoImplSyntaxic : InfoImpl
		{
			ParserState	state;
			muon::u32	readIndex;
			ASTNode*	node;
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
				INFO_IMPL->state = STATE_RUNNING;
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
					// Start parsing!
					ret = initParse(info);
					MUON_ASSERT(!g_OpAttribute.empty(), "Operator Precedence Stack is not empty!");
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

		// Precedence value set are arbritrary:
		// They must just reflect if an operator precedence is
		// higher / lower than another.
		// --
		//
		// We don't have to set the RBRACKET, as LBRACKET
		// operator attribute will be used for chained expression
		// like (((expr) op1 expr) op2 expr) where op1 perc. is < op2 prec
		g_OpAttribute[S_LBRACKET] = { 100, ASSOC_LEFT };

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

	void initLookupTable(hz::parser::Info& info)
	{
		// Statement Lookup Table
		memset(g_LookUpTable[STMT_DECL], LOOKUP_ERROR_INDEX, LOOKUP_ELEMENT_COUNT  * sizeof(muon::u32));

		// Function Declaration Table
		memset(g_LookUpTable[FUNC_DECL], LOOKUP_ERROR_INDEX, LOOKUP_ELEMENT_COUNT  * sizeof(muon::u32));

		// Class Declaration Table
		memset(g_LookUpTable[CLASS_DECL], LOOKUP_ERROR_INDEX, LOOKUP_ELEMENT_COUNT * sizeof(muon::u32));
	}

	bool initParse(hz::parser::Info& info)
	{

		return true;
	}

}
