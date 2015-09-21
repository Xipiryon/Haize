
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

	//********************************************************
	bool parseChunk(hz::parser::Info& info);
	hz::parser::ASTNode* parseBlock(hz::parser::Info& info);

	hz::parser::ASTNode* parseStmt(hz::parser::Info& info);
	hz::parser::ASTNode* parseExpr(hz::parser::Info& info);
	hz::parser::ASTNode* parsePrefixexpr(hz::parser::Info& info);

	hz::parser::ASTNode* parseOpAsn(hz::parser::Info& info);
	hz::parser::ASTNode* parseBinopMath(hz::parser::Info& info);
	hz::parser::ASTNode* parseBinopBitwise(hz::parser::Info& info);
	hz::parser::ASTNode* parseBinopLogic(hz::parser::Info& info);

	hz::parser::ASTNode* parseConstant(hz::parser::Info& info);
	hz::parser::ASTNode* parseVariable(hz::parser::Info& info);
	hz::parser::ASTNode* parseVariableLval(hz::parser::Info& info);
	//********************************************************
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
			muon::u32			currIndex;
			ASTNode*	node;
			muon::u32		depth;
			muon::u32		inputOffset;
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
				INFO_IMPL->currIndex = 0;
				INFO_IMPL->depth = 0;
				INFO_IMPL->inputOffset = 0;

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
					ret = parseChunk(info);
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

	void updateBinaryNodePrecedence(hz::parser::Info& info, hz::parser::ASTNode* node)
	{
		if(g_NodeOpAttribute.empty())
		{
			pushNodePrecedence(info, node);
			return;
		}

		hz::parser::ASTNode* nodeStack = g_NodeOpAttribute.back().node;
		const OpAttribute& op = g_OpAttribute[node->token.type];
		const OpAttribute& opStack = g_NodeOpAttribute.back().attr;

		if (checkForLowerPrecOrDiffAssoc(info, node))
		{
		}
		else
		{
		}
	}

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

#define TOK(Offset) ((INFO_IMPL->currIndex+Offset) < info.TokenList->size() ? (*info.TokenList)[INFO_IMPL->currIndex+Offset] : hz::parser::Token())
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
			INFO_IMPL->currIndex++;
		}
#else
		INFO_IMPL->currIndex += count;
#endif
	}

	//********************************************************
	bool parseChunk(hz::parser::Info& info)
	{
		hz::parser::ASTNode* chunk = info.ASTRoot;
		hz::parser::ASTNode* block = NULL;
		do
		{
			if (block = parseBlock(info))
			{
				chunk->addChild(block);
				continue;
			}
			switch (INFO_IMPL->state)
			{
				case hz::parser::STATE_SKIPPING:
					INFO_IMPL->state = hz::parser::STATE_RUNNING;
					continue;
				case hz::parser::STATE_DONE:
					break;
			}
		} while (block);
		// If there is no error message, then we're ok
		return info.error.message.empty();
	}

	hz::parser::ASTNode* parseBlock(hz::parser::Info& info)
	{
		if (EXPECT(0, S_EOF))
		{
			INFO_IMPL->state = hz::parser::STATE_DONE;
			return NULL;
		}
		return parseStmt(info);
	}

	hz::parser::ASTNode* parseStmt(hz::parser::Info& info)
	{
		// Allow empty statement
		if (EXPECT(0, S_SEPARATOR))
		{
			CONSUME(1); // Eat the separator
			INFO_IMPL->state = hz::parser::STATE_SKIPPING;
			return NULL;
		}

		if (EXPECTK(0, "return"))
		{
			CONSUME(1);	// Eat the return token
			auto ret = CREATENODE(hz::parser::NT_RETURN);
			if (auto expr = parseExpr(info))
			{
				ret->addChild(expr);
				return ret;
			}
			DELETENODE(ret);
			info.error.message = "";
			return NULL;
		}

		if (auto expr = parseExpr(info))
		{
			if (EXPECT(0, S_SEPARATOR))
			{
				CONSUME(1); // Eat the separator
				return expr;
			}

			DELETENODE(expr);
			ERR("Missing separator after expression: did you forget a ';' or a new line ?");
			return NULL;
		}

		muon::String errorMsg = "Unexpected symbol ";
		errorMsg += hz::parser::TokenTypeStr[TOK_TYPE(0)];
		ERR(errorMsg.cStr());
		return false;
	}

	hz::parser::ASTNode* parseExpr(hz::parser::Info& info)
	{
		hz::parser::ASTNode* prefixexpr = parsePrefixexpr(info);

		if (!prefixexpr)
		{
			if (EXPECT(0, S_LPARENT))
			{
				CONSUME(1);	// Eat '('
				if (auto expr = parseExpr(info))
				{
					if (TOK_TYPE(0) == hz::parser::S_RPARENT)
					{
						CONSUME(1);	// Eat ')'
						return expr;
					}
					else
					{
						ERR("Missing ')' !");
						DELETENODE(expr);
						return NULL;
					}
				}
				else
				{
					ERR("Unexpected '(' !");
					return NULL;
				}
			}
		}

		// prefixexpr [binop expr]
		hz::parser::ASTNode* binop = NULL;
		if (auto math = parseBinopMath(info))
		{
			binop = math;
		}
		else if (auto logic = parseBinopLogic(info))
		{
			binop = logic;
		}
		else if (auto bitwise = parseBinopBitwise(info))
		{
			binop = bitwise;
		}

		if (binop)
		{
			pushNodePrecedence(info, binop);
			// TODO: Check operator here
			if (auto expr = parseExpr(info))
			{
				binop->addChild(prefixexpr);
				binop->addChild(expr);
				return binop;
			}
			else
			{
				ERR("Expecting binary operator in expression");
				DELETENODE(binop);
				return NULL;
			}
		}

		if (!prefixexpr)
		{
			ERR("");
		}
		return prefixexpr;
	}

	hz::parser::ASTNode* parsePrefixexpr(hz::parser::Info& info)
	{
		if (EXPECT(0, MATH_ADD))
		{
			CONSUME(1);
			auto unary = CREATENODE(hz::parser::UNARY_PLUS);
			if (auto expr = parseExpr(info))
			{
				unary->addChild(expr);
				return unary;
			}
			DELETENODE(unary);
			return NULL;
		}

		if (EXPECT(0, MATH_SUB))
		{
			CONSUME(1);
			auto unary = CREATENODE(hz::parser::UNARY_MINUS);
			if (auto expr = parseExpr(info))
			{
				unary->addChild(expr);
				return unary;
			}
			DELETENODE(unary);
			return NULL;
		}

		if (auto constant = parseConstant(info))
		{
			return constant;
		}

		if (auto var = parseVariable(info))
		{
			return var;
		}

		return NULL;
	}

	hz::parser::ASTNode* parseOpAsn(hz::parser::Info& info)
	{
		return NULL;
	}

	hz::parser::ASTNode* parseBinopMath(hz::parser::Info& info)
	{
		if (TOK_TYPE(0) > hz::parser::E_MATH_OP_BEGIN && TOK_TYPE(0) < hz::parser::E_MATH_OP_END)
		{
			auto binop = CREATENODE(TOK_TYPE(0), hz::parser::TokenTypeStr[TOK_TYPE(0)]);
			CONSUME(1); // Eat the binop
			return binop;
		}
		return NULL;
	}

	hz::parser::ASTNode* parseBinopBitwise(hz::parser::Info& info)
	{
		if (TOK_TYPE(0) > hz::parser::E_BITWISE_OP_BEGIN && TOK_TYPE(0) < hz::parser::E_BITWISE_OP_END)
		{
			auto binop = CREATENODE(TOK_TYPE(0), hz::parser::TokenTypeStr[TOK_TYPE(0)]);
			CONSUME(1); // Eat the binop
			return binop;
		}
		return NULL;
	}

	hz::parser::ASTNode* parseBinopLogic(hz::parser::Info& info)
	{
		if (TOK_TYPE(0) > hz::parser::E_LOGIC_OP_BEGIN && TOK_TYPE(0) < hz::parser::E_LOGIC_OP_END)
		{
			auto binop = CREATENODE(TOK_TYPE(0), hz::parser::TokenTypeStr[TOK_TYPE(0)]);
			CONSUME(1); // Eat the binop
			return binop;
		}
		return NULL;
	}

	hz::parser::ASTNode* parseConstant(hz::parser::Info& info)
	{
		hz::parser::eTokenType nToken = TOK_TYPE(0);
		hz::parser::ASTNode* constant = NULL;
		switch (nToken)
		{
			case hz::parser::V_NIL:
				constant = CREATENODE(hz::parser::V_NIL);
				constant->token.line = TOK(0).line;
				constant->token.column = TOK(0).column;
				break;
			case hz::parser::V_TRUE:
				constant = CREATENODE(hz::parser::V_TRUE);
				constant->token.line = TOK(0).line;
				constant->token.column = TOK(0).column;
				break;
			case hz::parser::V_FALSE:
				constant = CREATENODE(hz::parser::V_FALSE);
				constant->token.line = TOK(0).line;
				constant->token.column = TOK(0).column;
				break;
			case hz::parser::V_NUMBER:
				constant = CREATENODE(hz::parser::V_NUMBER);
				constant->token.value = VARIANT(0);
				constant->token.line = TOK(0).line;
				constant->token.column = TOK(0).column;
				break;
			case hz::parser::V_STRING:
				constant = CREATENODE(hz::parser::V_STRING);
				constant->token.value.set<muon::String*>(VARIANT(0).get<muon::String*>());
				constant->token.line = TOK(0).line;
				constant->token.column = TOK(0).column;
				break;
			default:
				constant = NULL;
				break;
		}

		if (constant)
		{
			CONSUME(1); // Eat the constant
		}
		return constant;
	}

	hz::parser::ASTNode* parseVariable(hz::parser::Info& info)
	{
		hz::parser::ASTNode* lval = NULL;
		if (lval = parseVariableLval(info))
		{
			if (EXPECT(0, S_ACCESSOR))
			{
				CONSUME(1); // Eat the '.'
				auto accessor = CREATENODE(hz::parser::S_ACCESSOR);
				if (auto var = parseVariable(info))
				{
					accessor->addChild(lval);
					accessor->addChild(var);
					return accessor;
				}
				DELETENODE(lval);
				DELETENODE(accessor);
				ERR("Accessor fail");
				return NULL;
			}
		}
		return lval;
	}

	hz::parser::ASTNode* parseVariableLval(hz::parser::Info& info)
	{
		if (EXPECT(0, V_IDENTIFIER))
		{
			const hz::parser::Token& token = TOK(0);
			auto lval = CREATENODE(token);
			CONSUME(1); // Eat the identifier
			return lval;
		}
		return NULL;
	}
}
