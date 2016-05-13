#include <cstdlib>
#include <sstream>
#include <fstream>

#include <Muon/System/Log.hpp>
#include <Muon/System/Time.hpp>
#include "Haize/Runtime/OpCode.hpp"

#include "Haize/Context.hpp"

namespace hz
{
	ContextMemConfig::ContextMemConfig(m::u32 stackSize, m::u32 poolSize)
		: m_stackSize(stackSize)
		, m_poolSize(poolSize)
	{
	}
	Context::Context(const Module& name, ContextMemConfig memConfig)
		: m_stackAlloc(memConfig.m_stackSize)
		, m_poolAlloc(sizeof(m::u64), memConfig.m_poolSize)
		//	, m_stack(0)
	{
		//	m_byteCodeModules = MUON_NEW(ByteCodeModuleMap);
	}

	Context::~Context()
	{
		//	MUON_DELETE(m_byteCodeModules);
	}

	bool Context::prepare(const m::String& func)
	{
		return true;
	}

	bool Context::execute()
	{
		return true;
	}
}
