#ifndef _YACC_EXTERN_INCLUDE
#define _YACC_EXTERN_INCLUDE

// Functions definition
extern void custom_yymain(const char* buffer);
int processInput(char* buffer, int* numBytesRead, int maxBytesToRead);

// Global internal variables
namespace hz
{
	namespace parser
	{
		struct Info;
	}
}
extern hz::parser::Info* g_parseInfo;

extern int g_lineCount;
extern int g_charCount;
extern bool g_multiLineComment;
extern bool g_error;

// Functions from Flex/Yacc
extern void yyerror(const char*);
extern int yylex();
extern int yyparse();

#endif
