#include <sstream>
#include <Muon/System/Log.hpp>

#include "Haize/Parser/Priv/DefaultCompiler.hpp"

namespace hz
{
	namespace parser
	{
		namespace priv
		{
			bool DefaultCompiler::compile(Error& error)
			{
				m_tokenList = MUON_NEW(std::deque<parser::Token>);
				m_nodeRoot = MUON_NEW(parser::ASTNode);

				// avoid a macro, and avoid duplicating code
				auto clearVariable = [&]()
				{
					m_loadBuffer.clear();
					MUON_DELETE(m_tokenList);
					MUON_DELETE(m_nodeRoot);
				};

				if (!lexical(error))
				{
					clearVariable();
					return false;
				}
				m_loadBuffer.clear();

				if (!syntaxic(error))
				{
					clearVariable();
					return false;
				}
				m_tokenList->clear();

				if (!semantic(error))
				{
					clearVariable();
					return false;
				}

				clearVariable();
				return true;
			}
		}
	}
}
