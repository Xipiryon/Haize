#include "Haize/Engine.hpp"

namespace hz
{
	Engine::Engine()
	{
		m_moduleContext = MUON_NEW(ModuleContextMap);
	}

	Engine::~Engine()
	{
		for (auto it = m_moduleContext->begin(); it != m_moduleContext->end(); ++it)
		{
			Context* c = it->second;
			MUON_DELETE(c);
		}
		MUON_DELETE(m_moduleContext);
	}

	Context* Engine::createContext(const m::String& name)
	{
		Context* context = getContext(name);
		if (!context)
		{
			context = MUON_NEW(Context, name);
			m::String sName = name;
			(*m_moduleContext)[sName] = context;
		}
		return context;
	}

	Context* Engine::getContext(const m::String& name)
	{
		auto it = m_moduleContext->find(name);
		if (it != m_moduleContext->end())
		{
			return it->second;
		}
		return NULL;
	}

	bool Engine::destroyContext(const m::String& name)
	{
		auto it = m_moduleContext->find(name);
		if (it == m_moduleContext->end())
		{
			return false;
		}
		MUON_DELETE(it->second);
		m_moduleContext->erase(it);
		return true;
	}
}
