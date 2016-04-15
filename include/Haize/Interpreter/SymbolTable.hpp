#ifndef _HAIZE_SYMBOLTABLE_INCLUDE
#define _HAIZE_SYMBOLTABLE_INCLUDE

#include <unordered_map>
#include <Muon/String.hpp>
#include "Haize/Core/Define.hpp"

namespace hz
{
	class HAIZE_API SymbolTable
	{
	public:
		SymbolTable();
		~SymbolTable();

		m::u8 checkOrCreateRegister(const m::String&);
		void freeRegister(const m::String&);
		void freeRegister(m::u8);

	private:
		typedef std::unordered_map<m::String, m::u8> HashMapRegistry;
		typedef std::unordered_map<m::String, SymbolTable*> HashMapNamespace;

		HashMapRegistry* m_registers;
		HashMapNamespace* m_namespaces;
		m::u8 m_registerId;
	};
}

#endif
