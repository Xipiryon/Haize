#ifndef INCLUDE_HAIZE_CONTEXT_INCLUDED
#define INCLUDE_HAIZE_CONTEXT_INCLUDED

#include <vector>

#include <Muon/Variant.hpp>
#include <Muon/Memory/DoubleStackAllocator.hpp>
#include <Muon/Memory/PoolAllocator.hpp>

#include "Haize/Runtime/ByteCode.hpp"

#include "Haize/Module.hpp"
#include "Haize/Compiler/ASTNode.hpp"

namespace hz
{
	class HAIZE_API ContextMemConfig
	{
		friend class Context;
	public:
		ContextMemConfig(m::u32 variableSize = m::memory::KiB<64>::bytes
						 , m::u32 poolSize = m::memory::KiB<256>::bytes);

	private:
		m::u32 m_stackSize;
		m::u32 m_poolSize;
	};

	class HAIZE_API Context
	{
	public:

		Context(const Module& moduleRef, ContextMemConfig memConfig = ContextMemConfig());
		~Context();

		bool prepare(const m::String& func);

		bool execute();

	private:
		m::memory::DoubleStackAllocator m_stackAlloc;
		m::memory::PoolAllocator m_poolAlloc;

		m::Variant m_registers[ByteCode::REG_MAX_AVAILABLE];
		ByteCode m_instr;
		m::u32 m_stack;
	};
}
#endif
