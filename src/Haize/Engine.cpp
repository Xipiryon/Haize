
#include "Haize/Engine.hpp"

namespace hz
{
	Engine::Engine()
	{
		m_moduleContext = MUON_CNEW(ModuleContextMap);
	}

	Engine::~Engine()
	{
		for(auto it = m_moduleContext->begin(); it != m_moduleContext->end(); ++it)
		{
			Context* c = it->second;
			MUON_CDELETE(c);
		}
		MUON_CDELETE(m_moduleContext);
	}

	Context* Engine::createContext(const char* name)
	{
		Context* context = getContext(name);
		if(!context)
		{
			context = MUON_CNEW(Context, name);
			muon::String sName = name;
			(*m_moduleContext)[sName] = context;
		}
		return context;
	}

	Context* Engine::getContext(const char* name)
	{
		auto it = m_moduleContext->find(name);
		if(it != m_moduleContext->end())
		{
			return it->second;
		}
		return NULL;
	}

	bool Engine::removeContext(const char* name)
	{
		auto it = m_moduleContext->find(name);
		if(it == m_moduleContext->end())
		{
			return false;
		}
		MUON_CDELETE(it->second);
		m_moduleContext->erase(it);
		return true;
	}
}
