
#include <Muon/Memory/Allocator.hpp>
#include <Muon/System/Log.hpp>
#include "Haize/VM/SymbolTable.hpp"
#include "Haize/VM/ByteCode.hpp"

namespace hz
{
	SymbolTable::SymbolTable()
		: m_registers(MUON_CNEW(HashMapRegistry))
		, m_namespaces(MUON_CNEW(HashMapNamespace))
		, m_registerId(ByteCode::USABLE_REG)
	{
	}

	SymbolTable::~SymbolTable()
	{
		MUON_CDELETE(m_registers);
		for (auto it = m_namespaces->begin(); it != m_namespaces->end(); ++it)
		{
			MUON_CDELETE(it->second);
		}
		MUON_CDELETE(m_namespaces);
	}

	muon::u8 SymbolTable::checkOrCreateRegister(const muon::String& identifier)
	{
		if (m_registerId == hz::ByteCode::INVALID_REG)
		{
			muon::system::Log log("SymbolTable", muon::LOG_ERROR);
			log() << "Maximum number of internal registers reached!" << muon::endl;
			log() << "  This indicates that a block have 254 register used!" << muon::endl;
			log() << "  Previous register will be used. Value may be overwritten." << muon::endl;
			m_registerId = ByteCode::USABLE_REG;
		}

		auto it = m_registers->find(identifier);
		if (it != m_registers->end())
		{
			return it->second;
		}
		(*m_registers)[identifier] = m_registerId++;
		return (*m_registers)[identifier];
	}
	
	void SymbolTable::freeRegister(const muon::String& identifier)
	{
	}
	
	void SymbolTable::freeRegister(muon::u8 reg)
	{
	}
}
