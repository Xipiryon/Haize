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

	// ASCII Tree variables
	char g_Depth[2048];
	muon::u32 g_DepthIndex;

	void pushASCII(char c);
	void popASCII();
	void displayASCII(hz::parser::ASTNode* node);

	hz::parser::ASTNode* displayRecursive(std::ostream& graphviz, muon::u32 id, hz::parser::ASTNode* node);
	bool throwError(hz::parser::Info&, const char*, const hz::parser::Token&);

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
		enum ParserState
		{
			DONE,
			RUNNING,
			SKIPPING,
			ERROR,
		};

		/*
		* Custom parser::InfoImpl
		* Specific for Syntaxic parsing
		*/
		struct InfoImplSyntaxic : InfoImpl
		{
			ParserState state;
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
		static muon::system::Log log("ASCII", muon::LOG_DEBUG);
		log << hz::parser::TokenTypeStr[node->token.type] << muon::endl;
		for (muon::u32 i = 0; i < node->children->size(); ++i)
		{
			hz::parser::ASTNode* n = (*node->children)[i];
			if (i < node->children->size() - 1)
			{
#ifdef MUON_PLATFORM_WINDOWS
				log() << g_Depth << " |- " ;
#else
				log() << g_Depth << " ├─ " ;
#endif
				pushASCII('|');
			}
			else // Last element
			{
#ifdef MUON_PLATFORM_WINDOWS
				log() << g_Depth << " `- ";
#else
				log() << g_Depth << " └─ ";
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
#define EXPECTK(Offset, Name) ( TOK_TYPE(Offset) == hz::parser::S_KEYWORD ? (VARIANT(Offset).get<muon::String>() == Name) : false)

#define CREATENODE(Token) MUON_CNEW(hz::parser::ASTNode, Token)
#define DELETENODE(Node) MUON_CDELETE(Node)
#define CONSUME(Count) consume(info, Count)

	bool throwError(hz::parser::Info& info, const char* message, const hz::parser::Token& token)
	{
		info.error.message = message;
		info.error.line = token.line;
		info.error.column = token.column;
		INFO_IMPL->state = hz::parser::ERROR;
		return false;
	}

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
		return parseChunk(info);
	}

	//********************************************************
	bool parseChunk(hz::parser::Info& info)
	{
		hz::parser::ASTNode* block = NULL;
		do
		{
			if (block = parseBlock(info))
			{
				info.ASTRoot->addChild(block);
				continue;
			}
			switch (INFO_IMPL->state)
			{
				case hz::parser::SKIPPING:
				{
					INFO_IMPL->state = hz::parser::RUNNING;
					continue;
				}
				case hz::parser::ERROR:
					return false;
				case hz::parser::DONE:
					return true;
			}
		} while (block);
	}

	hz::parser::ASTNode* parseBlock(hz::parser::Info& info)
	{
		if (EXPECT(0, S_EOF))
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
			auto ret = CREATENODE(TOK(0));
			CONSUME(1);	// Eat the return token
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
			//ERR("Missing separator after expression: did you forget a ';' or a new line ?");
			return NULL;
		}

		muon::String errorMsg = "Unexpected symbol ";
		errorMsg += hz::parser::TokenTypeStr[TOK_TYPE(0)];
		//ERR(errorMsg.cStr());
		return NULL;
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
						//ERR("Missing ')' !");
						DELETENODE(expr);
						return NULL;
					}
				}
				else
				{
					//ERR("Unexpected '(' !");
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
			if (auto expr = parseExpr(info))
			{
				binop->addChild(prefixexpr);
				binop->addChild(expr);
				return binop;
			}
			else
			{
				//ERR("Expecting binary operator in expression");
				DELETENODE(binop);
				return NULL;
			}
		}

		if (!prefixexpr)
		{
			//ERR("");
		}
		return prefixexpr;
	}

	hz::parser::ASTNode* parsePrefixexpr(hz::parser::Info& info)
	{
		if (EXPECT(0, MATH_ADD))
		{
			auto unary = CREATENODE(TOK(0));
			CONSUME(1);
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
			auto unary = CREATENODE(TOK(0));
			CONSUME(1);
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
			auto binop = CREATENODE(TOK(0));
			CONSUME(1); // Eat the binop
			return binop;
		}
		return NULL;
	}

	hz::parser::ASTNode* parseBinopBitwise(hz::parser::Info& info)
	{
		if (TOK_TYPE(0) > hz::parser::E_BITWISE_OP_BEGIN && TOK_TYPE(0) < hz::parser::E_BITWISE_OP_END)
		{
			auto binop = CREATENODE(TOK(0));
			CONSUME(1); // Eat the binop
			return binop;
		}
		return NULL;
	}

	hz::parser::ASTNode* parseBinopLogic(hz::parser::Info& info)
	{
		if (TOK_TYPE(0) > hz::parser::E_LOGIC_OP_BEGIN && TOK_TYPE(0) < hz::parser::E_LOGIC_OP_END)
		{
			auto binop = CREATENODE(TOK(0));
			CONSUME(1); // Eat the binop
			return binop;
		}
		return NULL;
	}

	hz::parser::ASTNode* parseConstant(hz::parser::Info& info)
	{
		const hz::parser::Token& token = TOK(0);
		hz::parser::ASTNode* constant = NULL;
		switch (token.type)
		{
			case hz::parser::V_NIL:
				constant = CREATENODE(token);
				constant->token.line = TOK(0).line;
				constant->token.column = TOK(0).column;
				break;
			case hz::parser::V_TRUE:
				constant = CREATENODE(token);
				constant->token.line = TOK(0).line;
				constant->token.column = TOK(0).column;
				break;
			case hz::parser::V_FALSE:
				constant = CREATENODE(token);
				constant->token.line = TOK(0).line;
				constant->token.column = TOK(0).column;
				break;
			case hz::parser::V_NUMBER:
				constant = CREATENODE(token);
				//INFO_IMPL->node->token.value = VARIANT(0);
				constant->token.line = TOK(0).line;
				constant->token.column = TOK(0).column;
				break;
			case hz::parser::V_STRING:
				constant = CREATENODE(token);
				//INFO_IMPL->node->token.value.set<muon::String*>(VARIANT(0).get<muon::String*>());
				//INFO_IMPL->node->token.value = VARIANT(0);
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
				const hz::parser::Token& accessorToken = TOK(0);
				hz::parser::ASTNode* accessorNode = CREATENODE(accessorToken);
				CONSUME(1); // Eat the '.'
				if (auto var = parseVariable(info))
				{
					accessorNode->addChild(lval);
					accessorNode->addChild(var);
					return accessorNode;
				}
				DELETENODE(lval);
				DELETENODE(accessorNode);
				throwError(info, "Accessor failed", accessorToken);
				return NULL;
			}
		}
		return lval;
	}

	hz::parser::ASTNode* parseVariableLval(hz::parser::Info& info)
	{
		if (EXPECT(0, V_IDENTIFIER))
		{
			auto lval = CREATENODE(TOK(0));
			CONSUME(1); // Eat the identifier
			return lval;
		}
		return NULL;
	}
}
