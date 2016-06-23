#ifndef INCLUDE_HAIZE_CONTEXT_INCLUDED
#define INCLUDE_HAIZE_CONTEXT_INCLUDED

#include <vector>

#include <Muon/Memory/StackAllocator.hpp>
#include <Muon/Memory/PoolAllocator.hpp>

#include "Haize/Runtime/ByteCode.hpp"
#include "haize/Runtime/Variant.hpp"
#include "Haize/Module.hpp"
#include "Haize/Compiler/ASTNode.hpp"

namespace hz
{
	class HAIZE_API Context
	{
	public:

		Context(const Module& moduleRef);
		~Context();

		bool prepare(const m::String& func);
		bool execute();

		bool pushBoolean(bool value);
		bool pushInteger(m::i32 value);
		bool pushFloat(m::f32 value);
		bool pushString(const m::String& value);
		bool pushString(const char* value);
		bool pushObject(void* object);

		bool checkBoolean(m::u32 stack);
		bool checkInteger(m::u32 stack);
		bool checkFloat(m::u32 stack);
		bool checkString(m::u32 stack);
		bool checkObject(m::u32 stack);

		bool getAsBoolean(m::u32 stack);
		m::i32 getAsInteger(m::u32 stack);
		m::f32 getAsFloat(m::u32 stack);
		m::String getAsString(m::u32 stack);
		void* getAsObject(m::u32 stack);

	private:
		const Module& m_module;
		m::memory::StackAllocator m_stackAlloc;
		m::memory::PoolAllocator m_poolAlloc;

		Variant m_registers[ByteCode::REG_MAX_AVAILABLE];
	};
}
#endif
