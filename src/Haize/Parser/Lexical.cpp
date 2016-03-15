#include <sstream>
#include <Muon/System/Log.hpp>

#include "Haize/Parser/Compiler.hpp"

#include "./lemon/flex.haize.yy.hpp"

namespace utils
{
	int hex2int(char* str)
	{
		int val = 0;
		int i = 2; // Skip 0x
		int len = strlen(str);
		for(; i < len; ++i)
		{
			val <<= 4;
			if(str[i] >= '0' && str[i] <= '9')
			{
				val |= str[i] - '0';
			}
			else if(str[i] >= 'a' && str[i] <= 'f')
			{
				val |= str[i] - 'a' + 10;
			}
			else
			{
				val |= str[i] - 'A' + 10;
			}
		}
		return val;
	}

	int oct2int(char* str)
	{
		int val = 0;
		int i = 1; // Skip 0
		int len = strlen(str);
		for(; i < len; ++i)
		{
			val <<= 3;
			val |= str[i] - '0';
		}
		return val;
	}

	int bin2int(char* str)
	{
		int val= 0;
		int i = 2; // Skip 0b
		int len = strlen(str);
		for(; i < len; ++i)
		{
			val <<= 1;
			val |= (str[i] == '1' ? 1 : 0);
		}
		return val;
	}
}

namespace hz
{
	namespace parser
	{
		bool Compiler::lexical(Error& error)
		{
			error.clear();
			error.step = Error::COMPILATION;
			m_tokenList->clear();

			// ****************************
			yyscan_t scanner;
			//haize_flex_data data;
			//yylex_init_extra(&data, &scanner);
			yylex_init(&scanner);
			YY_BUFFER_STATE buffer = yy_scan_string(m_loadBuffer.cStr(), scanner);
			printf("yylex: %d\n", yylex(scanner));
			yy_delete_buffer(buffer, scanner);
			yylex_destroy(scanner);
			// ****************************

			parser::Token eof;
			eof.type = parser::S_EOF;
			m_tokenList->push_back(eof);

			error.clear();
			return true;
		}
	}
}
