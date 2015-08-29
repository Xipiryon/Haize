
#include <iostream>
#include <Muon/System/Log.hpp>

namespace hz
{
	namespace parser
	{
		struct Info;
	}
}
hz::parser::Info* g_parseInfo = NULL;

int g_lineCount = 1;
int g_charCount = 0;
bool g_multiLineComment = false;
bool g_error = false;

void yyerror(const char* err)
{
	g_error = true;
	muon::system::Log log("YACC", muon::LOG_ERROR);
	log() << "Error: \"" << "[Todo]" << "\" " << g_lineCount << ":" << g_charCount << muon::endl;
	log() << "=> \"" << err << "\" " << muon::endl;
}
