#include <Muon/System/Log.hpp>

#include "Haize/Parser/Compiler.hpp"

#include "generated/ExtraParserContext.hpp"
#include "generated/flex.l.hpp"
#include "generated/yacc.hpp"

namespace
{
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

void yyerror(const char* err)
{
	m::system::Log log("YACC", m::LOG_ERROR);
	//log() << "** Error at " << g_lineCount << ":" << g_charCount << " **" << m::endl;
	log() << "=> " << err << m::endl;
}

namespace hz
{
	namespace parser
	{
		bool Compiler::syntaxic(Error& error)
		{
			error.clear();
			error.step = Error::COMPILATION;

			// Clean children
			while (!m_nodeRoot->children->empty())
			{
				MUON_DELETE(m_nodeRoot->children->back());
				m_nodeRoot->children->pop_back();
			}

			// Reinit token
			m_nodeRoot->name = "#ROOT#";
			m_nodeRoot->token.type = 0;

			bool noError = true;
			m::u32 count = 0;
			parser::Token token;

			ExtraParserContext epc;
			/*
			void* parser = ParseAlloc(malloc);
			do
			{
			token = m_tokenList->at(count);
			printf("Sending Token: \"%s\" (%d) @ %d:%d\n", tokenValueToStr(token).cStr(), token.type, token.line, token.column);
			Parse(parser, token.type, "token", &epc);
			} while (++count < m_tokenList->size() && noError);
			ParseFree(parser, free);
			// */
			if (!noError)
			{
				unexpectedToken(token);
			}
			else
			{
				error.state = Error::SUCCESS;
			}

			exit(0);
			return noError;
		}
	}
}
