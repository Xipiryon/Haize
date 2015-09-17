
#include <Muon/Memory/Allocator.hpp>
#include <Muon/System/Log.hpp>
#include "Haize/VM/SymbolTable.hpp"
#include "Haize/VM/ByteCode.hpp"

namespace hz
{
	SymbolTable::SymbolTable()
		: _registers(MUON_CNEW(HashMapRegistry))
		, _namespaces(MUON_CNEW(HashMapNamespace))
		, _registerId(ByteCode::USABLE_REG)
	{
	}

	SymbolTable::~SymbolTable()
	{
		MUON_CDELETE(_registers);
		for (auto it = _namespaces->begin(); it != _namespaces->end(); ++it)
		{
			MUON_CDELETE(it->second);
		}
		MUON_CDELETE(_namespaces);
	}

	muon::u8 SymbolTable::checkOrCreateRegister(const muon::String& identifier)
	{
		if (_registerId == hz::ByteCode::INVALID_REG)
		{
			muon::system::Log log("SYMBOLTABLE", muon::LOG_ERROR);
			log() << "Maximum number of internal registers reached!" << muon::endl;
			log() << "  This indicates that a block have 254 register used!" << muon::endl;
			log() << "  Previous register will be used. Value may be overwritten." << muon::endl;
			_registerId = ByteCode::USABLE_REG;
		}

		auto it = _registers->find(identifier);
		if (it != _registers->end())
		{
			return it->second;
		}
		(*_registers)[identifier] = _registerId++;
		return (*_registers)[identifier];
	}
	
	void SymbolTable::freeRegister(const muon::String& identifier)
	{
	}
	
	void SymbolTable::freeRegister(muon::u8 reg)
	{
	}
}
