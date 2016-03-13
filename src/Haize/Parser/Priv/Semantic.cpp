#include <Muon/System/Log.hpp>

#include "Haize/Parser/Priv/DefaultCompiler.hpp"

namespace hz
{
	namespace parser
	{
		namespace priv
		{
			bool DefaultCompiler::semantic(Error& error)
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
}
