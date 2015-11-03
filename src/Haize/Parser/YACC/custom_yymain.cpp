#include <iostream>
#include <string>
#include <fstream>

#include <Muon/System/Log.hpp>
#include <Muon/String.hpp>
#include "Haize/Parser/Info.hpp"
#include "Extern.hpp"
#include "./generated/yacc.haize.hpp"

hz::parser::ASTNode* displayRecursive(std::ostream& graphviz, muon::u32 id, hz::parser::ASTNode* node)
{
	for (muon::u32 i = 0; i < id; ++i)
		graphviz << "  ";

	graphviz << "\"" << node << "\" [label=\"" << (node->name ? node->name : "#") << " (" << node->type << ")";
		muon::String str;
		switch(node->type)
		{
			case V_IDENT:
				str = node->value.get<muon::String>();
				graphviz << "\\n'" << str.cStr() << "'";
				break;
			case V_STRING:
			{
				str = node->value.get<muon::String>();
				str = str.replace("\n", "\\n");
				str = str.replace("\r", "\\r");
				str = str.replace("\t", "\\t");
				graphviz << "\\n'" << str.cStr() << "'";
			}
				break;
			case UNARY_SIGN:
				graphviz << "\\n'" << (node->value.get<bool>() ? "pos" : "neg") << "'";
				break;
			case V_TRUE:
			case V_FALSE:
				graphviz << "\\n'" << (node->value.get<bool>() ? "true" : "false") << "'";
				break;
			case V_INTEGER:
				graphviz << "\\n'" << node->value.get<muon::u32>() << "'";
				break;
			case V_FLOATING:
				graphviz << "\\n'" << node->value.get<muon::f32>() << "'";
				break;
			case V_NIL:
				graphviz << "\\n'nil'";
				break;
			default:
				break;
		}
	graphviz << "\"]" << std::endl;

	for (auto it = node->children->begin(); it != node->children->end(); ++it)
	{
		hz::parser::ASTNode* cnode = *it;
		displayRecursive(graphviz, id + 1, cnode);
		graphviz << "\"" << node << "\" -> \"" << cnode << "\"" << std::endl;
	}
	return NULL;
}

namespace
{
	int g_readOffset = 0;
	muon::String g_strArgument;
}

bool custom_yymain(const char* buffer)
{
	g_readOffset = 0;
	g_strArgument = buffer;
	g_strArgument += "\r\n";
	yyparse();

#ifdef MUON_DEBUG
	const char* file = "parse.syntaxic.gv";
	if (!g_error)
	{
		muon::system::Log log("YACC", muon::LOG_INFO);
		log() << "** Outputing AST to \"" << file << "\" file **" << muon::endl;

		std::ofstream graphviz(file, std::ios::trunc);
		if (graphviz)
		{
			graphviz << "digraph {" << std::endl;
			graphviz << "graph[splines = ortho]" << std::endl;
			graphviz << "node[shape = record]" << std::endl;
			if (g_parseInfo->ASTRoot)
			{
				displayRecursive(graphviz, 0, g_parseInfo->ASTRoot);
			}
			graphviz << "}" << std::endl;
		}
		graphviz.close();

#	ifdef MUON_PLATFORM_PC
		::system("dot.exe -Tpng parse.syntaxic.gv -o parse.syntaxic.png");
#	else
		::system("dot -Tpng parse.syntaxic.gv -o parse.syntaxic.png");
#	endif
	}
	else
	{
		muon::system::Log("YACC", muon::LOG_ERROR) << "** An error occured: no \"" << file << "\" will be generated **" << muon::endl;
		return false;
	}
#endif //MUON_DEBUG
	return true;
}

int processInput(char* buffer, int* numBytesRead, int maxBytesToRead )
{
	int numBytesToRead = maxBytesToRead;
	int bytesRemaining = g_strArgument.size() - g_readOffset;

	if (numBytesToRead > bytesRemaining)
	{
		numBytesToRead = bytesRemaining;
	}
	for (int i = 0; i < numBytesToRead; ++i)
	{
		buffer[i] = g_strArgument[g_readOffset+i];
	}
	*numBytesRead = numBytesToRead;
	g_readOffset += numBytesToRead;
	return 0;
}

void yyerror(const char* err)
{
	g_error = true;
	muon::system::Log log("YACC", muon::LOG_ERROR);
	log() << "** Error at " << g_lineCount << ":" << g_charCount << " **" << muon::endl;
	log() << "=> " << err << muon::endl;
}
