#include <fstream>
#include <sstream>
#include <Muon/System/Log.hpp>

#include "Haize/Parser/Compiler.hpp"

#include "generated/yacc.hpp"
#include "generated/flex.l.hpp"

namespace utils
{
	int hex2int(char* str)
	{
		int val = 0;
		int i = 2; // Skip 0x
		int len = strlen(str);
		for (; i < len; ++i)
		{
			val <<= 4;
			if (str[i] >= '0' && str[i] <= '9')
			{
				val |= str[i] - '0';
			}
			else if (str[i] >= 'a' && str[i] <= 'f')
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
		for (; i < len; ++i)
		{
			val <<= 3;
			val |= str[i] - '0';
		}
		return val;
	}

	int bin2int(char* str)
	{
		int val = 0;
		int i = 2; // Skip 0b
		int len = strlen(str);
		for (; i < len; ++i)
		{
			val <<= 1;
			val |= (str[i] == '1' ? 1 : 0);
		}
		return val;
	}

	m::String tokenValueToStr(const hz::parser::ASTNode* node)
	{
		char buffer[32];
		m::String tokstr;
		m::meta::MetaData* m = node->value.getMeta();
		if (MUON_META(m::String) == m)
		{
			tokstr = node->value.get<m::String>();
		}
		else if (MUON_META(m::f32) == m)
		{
			m::ftoa(node->value.get<m::f32>(), buffer);
			tokstr = buffer;
		}
		else if (MUON_META(m::i32) == m)
		{
			m::itoa((m::i64)node->value.get<m::i32>(), buffer);
			tokstr = buffer;
		}
		else
		{
			tokstr = "?";//hz::parser::TokenTypeStr[token.type];
		}
		return tokstr;
	}
}

namespace ascii
{
	// ASCII Tree variables
	char depth[2048];
	m::u32 depthIndex;

	void push(char c)
	{
		depth[depthIndex++] = ' ';
		depth[depthIndex++] = c;
		depth[depthIndex++] = ' ';
		depth[depthIndex++] = ' ';
		depth[depthIndex] = 0;
	}

	void pop()
	{
		depth[depthIndex -= 4] = 0;
	}

	void display(hz::parser::ASTNode* node)
	{
#if defined(HAIZE_DEBUG)
		printf(" %s (%s)\n", node->name.cStr(), utils::tokenValueToStr(node).cStr());
		for (m::u32 i = 0; i < node->children->size(); ++i)
		{
			hz::parser::ASTNode* n = (*node->children)[i];
			if (i < node->children->size() - 1)
			{
#if defined(MUON_PLATFORM_WINDOWS)
				printf(" %s |-", depth);
#else
				printf("%s ├─", depth);
#endif
				push('|');
			}
			else // Last element
			{
#if defined(MUON_PLATFORM_WINDOWS)
				printf(" %s '-", depth);
#else
				printf(" %s └─", depth);
#endif
				push(' ');
			}
			display(n);
			pop();
#endif
		}
	}
}

void yyerror(YYLTYPE *llocp, yyscan_t scanner, hz::parser::ASTNode* node, hz::Error* error, const char* err)
{
	error->line = llocp->first_line;
	error->column = llocp->first_column;
	error->message = err;
}

namespace hz
{
	namespace parser
	{
		Compiler::Compiler()
			: m_nodeRoot(MUON_NEW(parser::ASTNode))
		{
		}

		Compiler::~Compiler()
		{
			MUON_DELETE(m_nodeRoot);
		}

		bool Compiler::load(const char* buffer, Error& error)
		{
			error.clear();
			m_loadBuffer += buffer;
			error.state = Error::SUCCESS;
			return true;
		}

		bool Compiler::load(const m::String& file, Error& error)
		{
			error.clear();
			error.state = Error::ERROR;
			std::ifstream stream(file.cStr());
			if (!stream)
			{
				error.message = "Couldn't open the stream!";
				return false;
			}

			if (!stream.eof())
			{
				// Get stream size
				stream.seekg(0, stream.end);
				m::u64 length = (m::u64)stream.tellg();
				stream.seekg(0, stream.beg);

				char* buffer = (char*)malloc((m::u32)length + 1);
				stream.read(buffer, length);
				buffer[stream.gcount()] = 0;
				m_loadBuffer += buffer;
				free(buffer);

				error.state = Error::SUCCESS;
				return true;
			}
			error.message = "File stream seems to be empty: encountered EOF at beginning!";
			return false;
		}

		bool Compiler::compile(Error& error)
		{
			error.clear();
			error.step = Error::COMPILATION;
			error.state = Error::ERROR;

			// Clear previous compilation
			while (!m_nodeRoot->children->empty())
			{
				MUON_DELETE(m_nodeRoot->children->back());
				m_nodeRoot->children->pop_back();
			}
			m_nodeRoot->name = "#ROOT#";
			m_nodeRoot->type = NT_ROOT;

			// Start scanning
			yyscan_t scanner;

			yylex_init(&scanner);
			YY_BUFFER_STATE buffer = yy_scan_string(m_loadBuffer.cStr(), scanner);
			YYSTYPE yylval;
			memset(&yylval, 0, sizeof(YYSTYPE));

			int ret = yyparse(scanner, m_nodeRoot, &error);

			yy_delete_buffer(buffer, scanner);
			yylex_destroy(scanner);

			m_loadBuffer.clear();

			ascii::display(m_nodeRoot);
			if (ret != 0)
			{
				while (!m_nodeRoot->children->empty())
				{
					MUON_DELETE(m_nodeRoot->children->back());
					m_nodeRoot->children->pop_back();
				}
				error.state = Error::ERROR;
				return false;
			}

			// Semantic analysis
			// Skip empty AST
			if (m_nodeRoot->children->empty())
			{
				error.state = Error::SUCCESS;
				return true;
			}

			error.state = Error::SUCCESS;
			return true;
		}
	}
}
