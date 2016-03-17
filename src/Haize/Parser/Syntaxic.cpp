#include <Muon/System/Log.hpp>

#include "Haize/Parser/Compiler.hpp"

#include "lemon/lemon.h"
void* ParseAlloc(void* (*allocProc)(size_t));
void Parse(void*, int, const char*, bool&);
void ParseFree(void*, void(*freeProc)(void*));

namespace
{
	void unexpectedToken(const hz::parser::Token& token)
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

		m::system::Log error(m::LOG_ERROR);
		error()	<< "[" << token.line << ":" << token.column << "] "
				<< "Unexpected \"" << tokstr << "\""
				<< m::endl;
		//("[%d:%d] Unexpected token \"" + tokstr + "\"");
	}
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

			// ****************************
			void* parser = ParseAlloc(malloc);
			bool noError;
			m::u32 count = 0;
			parser::Token token;
			do
			{
				token = m_tokenList->at(count);
				Parse(parser, token.type, NULL, noError);
				printf("noError: %d\n", noError);
			}
			while (++count < m_tokenList->size() && noError);

			if(!noError)
			{
				unexpectedToken(token);
				return false;
			}

			//Parse(parser, 0, 0);
			ParseFree(parser, free);
			// ****************************
			exit(0);
			error.state = Error::SUCCESS;
			return true;
		}
	}
}
