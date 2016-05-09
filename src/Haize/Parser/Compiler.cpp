#include <fstream>
#include <sstream>
#include <Muon/System/Log.hpp>

#include "Haize/Parser/Compiler.hpp"

namespace hz
{
	namespace parser
	{
		Compiler::Compiler()
		{
			m_sections = MUON_NEW(std::vector<Section>);
		}

		Compiler::~Compiler()
		{
			MUON_DELETE(m_sections);
		}

		ByteCode* Compiler::compile(Error& error)
		{
			error.clear();
			error.step = Error::COMPILATION;
			error.state = Error::ERROR;

			m_tokenList = MUON_NEW(std::vector<parser::Token>);
			m_nodeRoot = MUON_NEW(parser::ASTNode);
			m_bytecode = NULL;
			// m_bytecode will be allocated in Semantic

			// avoid a macro, and avoid duplicating code
			auto clearVariable = [&]()
			{
				m_loadBuffer.clear();
				MUON_DELETE(m_tokenList);
				MUON_DELETE(m_nodeRoot);
				MUON_FREE(m_bytecode);
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
			return m_bytecode;
		}
	}
}
