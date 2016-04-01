#include <fstream>
#include <sstream>
#include <Muon/System/Log.hpp>

#include "Haize/Parser/Compiler.hpp"

namespace hz
{
	namespace parser
	{
		bool Compiler::compile(Error& error)
		{
			error.clear();
			error.step = Error::COMPILATION;
			error.state = Error::ERROR;

			m_tokenList = MUON_NEW(std::vector<parser::Token>);
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

			if (!syntaxic(error))
			{
				clearVariable();
				return false;
			}

			if (!semantic(error))
			{
				clearVariable();
				return false;
			}

			clearVariable();
			error.state = Error::SUCCESS;
			return true;
		}
	}
}
