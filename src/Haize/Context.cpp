#include <cstdlib>
#include <sstream>
#include <fstream>

#include <Muon/System/Log.hpp>
#include <Muon/System/Time.hpp>
#include "Haize/Runtime/OpCode.hpp"

#include "Haize/Engine.hpp"
#include "Haize/Context.hpp"

namespace hz
{
	ContextMemConfig::ContextMemConfig(m::u32 stackSize, m::u32 poolSize)
		: m_stack(stackSize)
		, m_pool(sizeof(m::u64), poolSize)
	{
	}

	Context::Context(const m::String& name, ContextMemConfig memConfig)
		: m_name(name)
		, m_memConfig(memConfig)
		//	, m_stack(0)
	{
		//	m_byteCodeModules = MUON_NEW(ByteCodeModuleMap);
		m_error.clear();
	}

	Context::~Context()
	{
		//	MUON_DELETE(m_byteCodeModules);
	}

	//==================================
	//			CONTEXT ATTRIBUTE
	//==================================

	const char* Context::getName() const
	{
		return m_name.cStr();
	}

	Error Context::getLastError() const
	{
		return m_error;
	}

	//==================================
	//			COMPILATION
	//==================================

	bool Context::load(const m::String& filename)
	{
		m_error.clear();
		m_error.step = Error::LOADING;
		return m_compiler.load(filename, m_error);
	}

	bool Context::compile()
	{
		if (m_error.state == Error::ERROR && m_error.step == Error::LOADING)
		{
			return false;
		}
		m_error.clear();
		m_error.step = Error::COMPILATION;

		// ByteCode is updated directly in the compile() function
		return m_compiler.compile(m_error, &m_byteCode);
	}

	//==================================
	//			EXECUTION FLOW
	//==================================

	bool Context::prepare(const m::String& func)
	{
		if (m_error.state == Error::ERROR && m_error.step == Error::COMPILATION)
		{
			return false;
		}
		m_error.clear();
		m_error.step = Error::PREPARATION;
		return true;
	}

	bool Context::execute()
	{
		if (m_error.state == Error::ERROR && m_error.step == Error::PREPARATION)
		{
			return false;
		}
		m_error.clear();
		m_error.step = Error::EXECUTION;
		return true;
	}

	//==================================
	//			EXECUTION
	//==================================

	bool Context::eval(const char* code, bool storeCode /* = false*/)
	{
		m_error.step = Error::COMPILATION;

		return false;
	}

	bool Context::run(const ByteCode* buffer)
	{
		m_error.step = Error::EXECUTION;

		return false;
	}
}
