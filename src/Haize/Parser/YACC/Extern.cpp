
#include <iostream>
#include <Muon/System/Log.hpp>
#include "Haize/Parser/ASTNode.hpp"

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
