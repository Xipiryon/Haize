
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
	bool parseChunk(hz::parser::Info& info);
	bool parseBlock(hz::parser::Info& info);

	bool parseStmt(hz::parser::Info& info);
	bool parseExpr(hz::parser::Info& info);
	bool parsePreexpr(hz::parser::Info& info);

	bool parseOpAsn(hz::parser::Info& info);
	bool parseOpMath(hz::parser::Info& info);
	bool parseOpBitw(hz::parser::Info& info);
	bool parseOpCmp(hz::parser::Info& info);

	bool parseConstant(hz::parser::Info& info);
	bool parseVariable(hz::parser::Info& info);
	bool parseVariableLval(hz::parser::Info& info);
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
		struct InfoImplSyntaxic : InfoImpl
		{
			u32			nextIndex;
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
					while(!info.ASTRoot->children->empty())
					{
						MUON_CDELETE(info.ASTRoot->children->back());
					}
				}
				info.ASTRoot->name = "NT_PROGRAM";
				info.ASTRoot->token = Token(NT_CHUNK);

				info.impl = MUON_CNEW(InfoImplSyntaxic);
				INFO_IMPL->node = info.ASTRoot;
				INFO_IMPL->nextIndex = 0;
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
					if (!parseChunk(info))
					{
						ret = false;
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
				muon::system::Log("SYNTAXIC", muon::LOG_INFO) << "** Outputing AST to \"AST_Graph.gv\" file **" << muon::endl;

				std::ofstream graphviz("AST_Graph.gv", std::ios::trunc);
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
				::system("dot.exe -Tpng AST_Graph.gv -o AST_Graph.png");
#else
				::system("dot -Tpng AST_Graph.gv -o AST_Graph.png");
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
		printf( "%s \n", hz::parser::TokenTypeStr[node->token.type]);
		for (muon::u32 i = 0; i < node->children->size(); ++i)
		{
			hz::parser::ASTNode* n = (*node->children)[i];
			if(i < node->children->size()-1)
			{
				printf( "%s ├─", g_Depth);
				pushASCII('|');
			}
			else // Last element
			{
				printf( "%s └─", g_Depth);
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
	hz::parser::eTokenType next(hz::parser::Info& info)
	{
		if (INFO_IMPL->nextIndex < info.TokenList->size())
		{
			return (*info.TokenList)[INFO_IMPL->nextIndex].type;
		}
		return hz::parser::S_INVALID;
	}
#define NEXT next(info)

	muon::meta::Variant value(hz::parser::Info& info)
	{
		if (INFO_IMPL->nextIndex < info.TokenList->size())
		{
			return (*info.TokenList)[INFO_IMPL->nextIndex].value;
		}
		return muon::meta::Variant();
	}
#define VALUE value(info)

	bool expect(hz::parser::Info& info, hz::parser::eTokenType type)
	{
		return (next(info) == type);
	}
#define EXPECT(Type) expect(info, hz::parser::Type)

	bool expectKeyword(hz::parser::Info& info, const char* str)
	{
		if(next(info) == hz::parser::S_KEYWORD)
		{
			hz::parser::Token& token = (*info.TokenList)[INFO_IMPL->nextIndex];
			muon::String* val = token.value.get<muon::String*>();
			return (*val == str);
		}
		return false;
	}
#define EXPECTK(Name) expectKeyword(info, Name)

	hz::parser::ASTNode* addNode(hz::parser::Info& info, hz::parser::eTokenType type, const char* name)
	{
		hz::parser::ASTNode* node = INFO_IMPL->node->addChild(type, name);
		INFO_IMPL->node = node;
		return node;
	}
#define ADD(Type) addNode(info, hz::parser::Type, #Type)

	hz::parser::ASTNode* consume(hz::parser::Info& info, hz::parser::eTokenType type, const char* name)
	{
		INFO_IMPL->nextIndex++;
		return addNode(info, type, name);
	}
#define CONSUME(Type) consume(info, hz::parser::Type, #Type)

	hz::parser::ASTNode* rewind(hz::parser::Info& info)
	{
		return INFO_IMPL->node = INFO_IMPL->node->parent;
	}
#define REWIND rewind(info)

	hz::parser::ASTNode* rollback(hz::parser::Info& info)
	{
		hz::parser::ASTNode* n = INFO_IMPL->node;
		REWIND;
		INFO_IMPL->node->deleteChild(n);
		return INFO_IMPL->node;
	}
#define ROLLBACK rollback(info);

	//********************************************************
	bool parseChunk(hz::parser::Info& info)
	{
		INFO_IMPL->node = info.ASTRoot;
		parseBlock(info);
		// If there is no error message, then we're ok
		return info.error.message.empty();
	}

	bool parseBlock(hz::parser::Info& info)
	{
		bool ok = true;
		while (ok)
		{
			ADD(NT_STMT_DECL);
			ok = parseStmt(info);
			REWIND;
		} 
		//Remvoe the last NT_STMT
		INFO_IMPL->node->deleteChild(INFO_IMPL->node->children->back());
		return true;
	}

	bool parseStmt(hz::parser::Info& info)
	{
		if(EXPECT(S_SEPARATOR))
		{
			return true;
		}

		if(EXPECTK("return"))
		{
			CONSUME(NT_RETURN);
			parseExpr(info);
			REWIND;
			return true;
		}
		return false;
	}

	bool parseExpr(hz::parser::Info& info)
	{
		if(parsePreexpr(info))
		{
			return true;
		}
		return false;
	}

	bool parsePreexpr(hz::parser::Info& info)
	{
		if(EXPECT(MATH_ADD))
		{
			ADD(UNARY_PLUS);
			bool ok = parseExpr(info);
			REWIND;
			return ok;
		}

		if(EXPECT(MATH_SUB))
		{
			ADD(UNARY_MINUS);
			bool ok = parseExpr(info);
			REWIND;
			return ok;
		}

		if (parseConstant(info))
		{
			return true;
		}

		if (parseVariable(info))
		{
			return true;
		}
		return false;
	}

	bool parseOpAsn(hz::parser::Info& info)
	{
		return false;
	}

	bool parseOpMath(hz::parser::Info& info)
	{
		return false;
	}

	bool parseOpBitw(hz::parser::Info& info)
	{
		return false;
	}

	bool parseOpCmp(hz::parser::Info& info)
	{
		return false;
	}

	bool parseConstant(hz::parser::Info& info)
	{
		hz::parser::eTokenType nToken = NEXT;
		switch (nToken)
		{
			case hz::parser::V_NIL:
				ADD(V_NIL);
				break;
			case hz::parser::V_TRUE:
				ADD(V_TRUE);
				break;
			case hz::parser::V_FALSE:
				ADD(V_FALSE);
				break;
			case hz::parser::V_NUMBER:
				ADD(V_NUMBER);
				INFO_IMPL->node->token.value = VALUE;
				break;
			case hz::parser::V_STRING:
				ADD(V_STRING);
				INFO_IMPL->node->token.value.set<muon::String*>(VALUE.get<muon::String*>());
				break;
			default:
				return false;
		}
		REWIND;
		return true;
	}

	bool parseVariable(hz::parser::Info& info)
	{
		if(parseVariableLval(info))
		{
			if(EXPECT(S_ACCESSOR))
			{
				CONSUME(S_ACCESSOR);
				if(parseVariableLval(info))
				{
					REWIND;
					return true;
				}
				ROLLBACK;
				return false;
			}
		}
		return false;
	}

	bool parseVariableLval(hz::parser::Info& info)
	{
		if(EXPECT(V_IDENTIFIER))
		{
			ADD(V_IDENTIFIER);
			REWIND;
			return true;
		}
		return false;
	}
}
