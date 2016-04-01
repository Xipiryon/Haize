#include <Muon/System/Log.hpp>

#include "Haize/Parser/Compiler.hpp"

namespace hz
{
	namespace parser
	{
		bool Compiler::semantic(Error& error)
		{
			// Skip empty AST
			if (m_nodeRoot->children->empty())
			{
				return true;
			}
			error.clear();
			error.step = Error::COMPILATION;

			return true;
		}
	}
}
