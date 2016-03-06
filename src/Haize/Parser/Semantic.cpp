
#include <Muon/System/Log.hpp>

#include "Haize/Context.hpp"

namespace hz
{
	bool Context::parseSemantic()
	{
		// Skip empty AST
		if(m_nodeRoot->children->empty())
		{
			return true;
		}

		clearError(false);
		m_error.message = "Semantic not implemented yet";
		return false;
	}
}
