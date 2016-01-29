
#include "Haize/Engine.hpp"

namespace hz
{
	Engine::Engine()
	{
		m_moduleContext = MUON_NEW(ModuleContextMap);
	}

	Engine::~Engine()
	{

		MUON_DELETE(m_moduleContext);
	}
}
