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
				error.state = Error::ERROR;
				error.message = "Semantic not implemented yet";
				return false;
			}
		}
	}
}
