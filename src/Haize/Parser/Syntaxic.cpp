
#include <map>
#include <stack>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <Muon/Memory/Allocator.hpp>
#include <Muon/System/Log.hpp>
#include "Haize/Parser/ASTNode.hpp"
#include "Haize/Parser/Syntaxic.hpp"

namespace
{
	using namespace muon;

	// ASCII Tree variables
	char g_Depth[2048];
	u32 g_DepthIndex;

	void pushASCII(char c);
	void popASCII();
	void displayASCII(hz::parser::ASTNode* node);

	hz::parser::ASTNode* displayRecursive(std::ostream& graphviz, u32 id, hz::parser::ASTNode* node);

	void initPrecedenceAssoc(hz::parser::Info&);

	//********************************************************
	hz::parser::ASTNode* parseChunk(hz::parser::Info& info);
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
	enum TokenAssoc
	{
		LEFT,
		RIGHT,
	};

	typedef std::map<hz::parser::eTokenType, u32> TokenPrecedenceMap;
	typedef std::map<hz::parser::eTokenType, TokenAssoc> TokenAssociativityMap;
	typedef std::map<hz::parser::eTokenType, std::vector<hz::parser::eTokenType>> SyntaxicTable;
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
			RUNNING,
			SKIPPING,
			DONE
		};
		struct InfoImplSyntaxic : InfoImpl
		{
			ParserState	state;
			u32			currIndex;
			ASTNode*	node;
			u32		depth;
			u32		inputOffset;
			TokenPrecedenceMap* precedence;
			TokenAssociativityMap* associativity;
			SyntaxicTable* syntaxicTable;
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
				info.ASTRoot->name = "NT_PROGRAM";
				info.ASTRoot->token = Token(NT_CHUNK);

				info.impl = MUON_CNEW(InfoImplSyntaxic);
				INFO_IMPL->node = info.ASTRoot;
				INFO_IMPL->state = RUNNING;
				INFO_IMPL->currIndex = 0;
				INFO_IMPL->depth = 0;
				INFO_IMPL->inputOffset = 0;
				INFO_IMPL->precedence = MUON_CNEW(TokenPrecedenceMap);
				INFO_IMPL->associativity = MUON_CNEW(TokenAssociativityMap);
				INFO_IMPL->syntaxicTable = MUON_CNEW(SyntaxicTable);

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
					hz::parser::ASTNode* chunk = parseChunk(info);
					ret = (chunk != NULL && info.error.message.empty());
					if (ret)
					{
						info.ASTRoot->addChild(chunk);
					}
					else
					{
						MUON_CDELETE(chunk);
					}
				}

				MUON_CDELETE(INFO_IMPL->precedence);
				MUON_CDELETE(INFO_IMPL->associativity);
				MUON_CDELETE(INFO_IMPL->syntaxicTable);
				MUON_CDELETE(INFO_IMPL);
				return ret;
			}

			void display(parser::Info& info)
			{
#ifdef MUON_DEBUG
				muon::system::Log("SYNTAXIC", muon::LOG_INFO) << "** Outputing AST to \"parse.syntaxic.gv\" file **" << muon::endl;

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

#ifdef MUON_PLATFORM_PC
				::system("dot.exe -Tpng parse.syntaxic.gv -o parse.syntaxic.png");
#else
				::system("dot -Tpng parse.syntaxic.gv -o parse.syntaxic.png");
#endif

				muon::system::Log("SYNTAXIC", muon::LOG_INFO) << "** Displaying AST in ASCII **" << muon::endl;
				displayASCII(info.ASTRoot);
#endif
			}
		}
	}
}

namespace
{
	void initPrecedenceAssoc(hz::parser::Info& info)
	{
		INFO_IMPL->precedence->clear();
		INFO_IMPL->associativity->clear();

		using namespace hz::parser;
		for (u32 i = 0; i < eTokenType::TOTAL_COUNT; ++i)
		{
			eTokenType t = (eTokenType)i;
			switch (t)
			{
				case eTokenType::S_ACCESSOR:
					(*INFO_IMPL->precedence)[t] = 30;
					(*INFO_IMPL->associativity)[t] = TokenAssoc::RIGHT;
					break;
				case eTokenType::S_LBRACKET:
				case eTokenType::S_RBRACKET:
					(*INFO_IMPL->precedence)[t] = 30;
					break;
				case eTokenType::UNARY_PLUS:
				case eTokenType::UNARY_MINUS:
				case eTokenType::MATH_INC:
				case eTokenType::MATH_DEC:
					(*INFO_IMPL->precedence)[t] = 25;
					(*INFO_IMPL->associativity)[t] = TokenAssoc::RIGHT;
					break;
				case eTokenType::MATH_MUL:
				case eTokenType::MATH_DIV:
				case eTokenType::MATH_MOD:
					(*INFO_IMPL->precedence)[t] = 20;
					break;
				case eTokenType::MATH_ADD:
				case eTokenType::MATH_SUB:
					(*INFO_IMPL->precedence)[t] = 10;
					break;
				case eTokenType::MATH_ASN:
					(*INFO_IMPL->precedence)[t] = 1;
					(*INFO_IMPL->associativity)[t] = TokenAssoc::RIGHT;
					break;
				default:
					(*INFO_IMPL->associativity)[t] = TokenAssoc::LEFT;
					(*INFO_IMPL->precedence)[t] = 0;
					break;
			}
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
				printf("%s ├─", g_Depth);
				pushASCII('|');
			}
			else // Last element
			{
				printf("%s └─", g_Depth);
				pushASCII(' ');
			}
			displayASCII(n);
			popASCII();
		}
	}

	hz::parser::ASTNode* displayRecursive(std::ostream& graphviz, u32 id, hz::parser::ASTNode* node)
	{
		for (u32 i = 0; i < id; ++i)
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
#ifdef MUON_DEBUG
		muon::system::Log c("Consuming", muon::LOG_DEBUG);
		for(muon::u32 i = 0; i < count; ++i)
		{
			c << hz::parser::TokenTypeStr[TOK_TYPE(0)] << muon::endl;
			INFO_IMPL->currIndex++;
		}
#else
		INFO_IMPL->currIndex += count;
#endif
	}

	//********************************************************
	hz::parser::ASTNode* parseChunk(hz::parser::Info& info)
	{
		hz::parser::ASTNode* chunk = CREATENODE(hz::parser::NT_CHUNK);
		hz::parser::ASTNode* block = NULL;
		do
		{
			if (block = parseBlock(info))
			{
				CREATENODE(hz::parser::NT_CHUNK);
				chunk->addChild(block);
				continue;
			}
			switch (INFO_IMPL->state)
			{
				case hz::parser::SKIPPING:
					INFO_IMPL->state = hz::parser::RUNNING;
					continue;
				case hz::parser::DONE:
					break;
			}
		} while (block);
		// If there is no error message, then we're ok
		return chunk;
	}

	hz::parser::ASTNode* parseBlock(hz::parser::Info& info)
	{
		if(EXPECT(0, S_EOF))
		{
			INFO_IMPL->state = hz::parser::DONE;
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
			INFO_IMPL->state = hz::parser::SKIPPING;
			return NULL;
		}

		if (EXPECTK(0, "return"))
		{
			CONSUME(1);	// Eat the return token
			auto ret = CREATENODE(hz::parser::NT_RETURN);
			if(auto expr = parseExpr(info))
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

		if(!prefixexpr)
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

		if(binop)
		{
			if(auto expr = parseExpr(info))
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
		
		if(!prefixexpr)
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
			if(auto expr = parseExpr(info))
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
				INFO_IMPL->node->token.value = VARIANT(0);
				constant->token.line = TOK(0).line;
				constant->token.column = TOK(0).column;
				break;
			case hz::parser::V_STRING:
				constant = CREATENODE(hz::parser::V_STRING);
				INFO_IMPL->node->token.value.set<muon::String*>(VARIANT(0).get<muon::String*>());
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
