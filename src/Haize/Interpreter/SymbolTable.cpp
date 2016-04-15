
#include <Muon/Memory/Allocator.hpp>
#include <Muon/System/Log.hpp>
#include "Haize/Interpreter/SymbolTable.hpp"
#include "Haize/Interpreter/ByteCode.hpp"

namespace hz
{
	SymbolTable::SymbolTable()
		: m_registers(MUON_NEW(HashMapRegistry))
		, m_namespaces(MUON_NEW(HashMapNamespace))
		, m_registerId(ByteCode::REG_USABLE)
	{
	}

	SymbolTable::~SymbolTable()
	{
		MUON_DELETE(m_registers);
		for (auto it = m_namespaces->begin(); it != m_namespaces->end(); ++it)
		{
			MUON_DELETE(it->second);
		}
		MUON_DELETE(m_namespaces);
	}

	m::u8 SymbolTable::checkOrCreateRegister(const m::String& identifier)
	{
		if (m_registerId == hz::ByteCode::REG_INVALID)
		{
			m::system::Log log("SymbolTable", m::LOG_ERROR);
			log() << "Maximum number of internal registers reached!" << m::endl;
			log() << "  This indicates that a block have " << ByteCode::REG_MAX_AVAILABLE <<" register used!" << m::endl;
			log() << "  Previous register will be used. Value may be overwritten." << m::endl;
			m_registerId = ByteCode::REG_USABLE;
		}

		auto it = m_registers->find(identifier);
		if (it != m_registers->end())
		{
			return it->second;
		}
		(*m_registers)[identifier] = m_registerId++;
		return (*m_registers)[identifier];
	}

	void SymbolTable::freeRegister(const m::String& identifier)
	{
	}

	void SymbolTable::freeRegister(m::u8 reg)
	{
	}
}
