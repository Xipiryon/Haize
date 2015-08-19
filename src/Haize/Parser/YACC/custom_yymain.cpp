#include <iostream>
#include <string>
#include <fstream>

#include <Muon/System/Log.hpp>
#include "Haize/Parser/Info.hpp"
#include "Extern.hpp"
#include "generated/yacc.haize.hpp"

hz::parser::ASTNode* displayRecursive(std::ostream& graphviz, muon::u32 id, hz::parser::ASTNode* node)
{
	for (muon::u32 i = 0; i < id; ++i)
		graphviz << "  ";

	graphviz << "\"" << node << "\" [label=\""
		<< (node->name ? node->name : "#") << " (" << node->type << ")";
		//<< " #" << node->token.line << ":" << node->token.column
		muon::String str;
		switch(node->type)
		{
			case IDENTIFIER:
				str = *node->value.get<muon::String*>();
				graphviz << "\\n'" << str.cStr() << "'";
				break;
			case STRING:
			{
				str = *node->value.get<muon::String*>();
				str = str.replace("\n", "\\n");
				str = str.replace("\r", "\\r");
				str = str.replace("\t", "\\t");
				graphviz << "\\n'" << str.cStr() << "'";
			}
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
	std::string g_strArgument;
}

void custom_yymain(const char* buffer)
{
	g_readOffset = 0;
	g_strArgument = buffer;
	yyparse();

#ifdef MUON_DEBUG
	if (!g_error)
	{
		// Let's output this on a Graphviz diagram for debug purposes
		muon::system::Log("AST", muon::LOG_INFO) << "** Outputing AST to \"AST_Graph.gv\" file **" << muon::endl;

		std::ofstream graphviz("AST_Graph.gv", std::ios::trunc);
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

#ifdef MUON_PLATFORM_WINDOWS
		::system("dot.exe -Tpng AST_Graph.gv -o AST_Graph.png");
#else
		::system("dot -Tpng AST_Graph.gv -o AST_Graph.png");
#endif
	}
	else
	{
		muon::system::Log("AST", muon::LOG_ERROR) << "** An error occured: no \"AST_Graph.gv\" will be generated **" << muon::endl;
#ifdef MUON_PLATFORM_WINDOWS
		::system("PAUSE");
#endif
	}
#endif
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