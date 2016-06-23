#include <cstdlib>
#include <sstream>
#include <fstream>

#include <Muon/System/Log.hpp>
#include <Muon/System/Time.hpp>
#include "Haize/Runtime/OpCode.hpp"

#include "Haize/Context.hpp"

namespace hz
{
	Context::Context(const Module& module)
		: m_module(module)
		, m_stackAlloc(HAIZE_CONTEXT_STACKSIZE)
		, m_poolAlloc(sizeof(m::u32), HAIZE_CONTEXT_POOLSIZE)
	{
	}

	Context::~Context()
	{
	}

	bool Context::prepare(const m::String& func)
	{
		return true;
	}

	bool Context::execute()
	{
		return true;
	}

	bool Context::pushBoolean(bool value)
	{
		return true;
	}

	bool Context::pushInteger(m::i32 value)
	{
		return true;
	}

	bool Context::pushFloat(m::f32 value)
	{
		return true;
	}

	bool Context::pushString(const m::String& value)
	{
		return true;
	}

	bool Context::pushString(const char* value)
	{
		return true;
	}

	bool Context::pushObject(void* object)
	{
		return true;
	}

	bool Context::checkBoolean(m::u32 stack)
	{
		return true;
	}

	bool Context::checkInteger(m::u32 stack)
	{
		return true;
	}

	bool Context::checkFloat(m::u32 stack)
	{
		return true;
	}

	bool Context::checkString(m::u32 stack)
	{
		return true;
	}

	bool Context::checkObject(m::u32 stack)
	{
		return true;
	}

	bool Context::getAsBoolean(m::u32 stack)
	{
		return true;
	}

	m::i32 Context::getAsInteger(m::u32 stack)
	{
		return 0;
	}

	m::f32 Context::getAsFloat(m::u32 stack)
	{
		return 0.0f;
	}

	m::String Context::getAsString(m::u32 stack)
	{
		return "";
	}

	void* Context::getAsObject(m::u32 stack)
	{
		return NULL;
	}
}
