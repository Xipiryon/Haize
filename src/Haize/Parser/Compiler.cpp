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

	m::String tokenValueToStr(const hz::parser::Token& token)
	{
		char buffer[32];
		m::String tokstr;
		m::meta::MetaData* m = token.value.getMeta();
		if (MUON_META(m::String) == m)
		{
			tokstr = token.value.get<m::String>();
		}
		else if (MUON_META(m::f32) == m)
		{
			m::ftoa(token.value.get<m::f32>(), buffer);
			tokstr = buffer;
		}
		else if (MUON_META(m::i32) == m)
		{
			m::itoa((m::i64)token.value.get<m::i32>(), buffer);
			tokstr = buffer;
		}
		else
		{
			tokstr = "?";//hz::parser::TokenTypeStr[token.type];
		}
		return tokstr;
	}

	void unexpectedToken(const hz::parser::Token& token)
	{
		m::String val = tokenValueToStr(token);
		m::system::Log error(m::LOG_ERROR);
		error() << "[" << "token.section" << "] "
			<< "Unexpected \"" << val.cStr() << "\" token @ "
			<< token.line << ":" << token.column
			<< m::endl;
		//("[%d:%d] Unexpected token \"" + tokstr + "\"");
	}
}

void yyerror(YYLTYPE *llocp, void* token, const char* err)
{
	m::system::Log log("Compiler", m::LOG_ERROR);
	log() << "\"" << err << "\" @ " << llocp->first_line << ":" << llocp->first_column << m::endl;
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
			m_nodeRoot->token.type = NT_ROOT;

			// Start scanning
			yyscan_t scanner;
			yylex_init(&scanner);
			YY_BUFFER_STATE buffer = yy_scan_string(m_loadBuffer.cStr(), scanner);
			YYSTYPE yylval;
			YYLTYPE yylpos;
			memset(&yylval, 0, sizeof(YYSTYPE));
			memset(&yylpos, 0, sizeof(YYLTYPE));

			int id;
			do
			{
				id = yylex(&yylval, &yylpos, scanner);
				yyparse(scanner);
				yylpos.first_column += strlen(yyget_text(scanner));
			} while (id != 0);

			yy_delete_buffer(buffer, scanner);
			yylex_destroy(scanner);

			m_loadBuffer.clear();
			system("PAUSE");
			exit(0);

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
