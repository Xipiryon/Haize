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

		muon::u8 checkOrCreateRegister(const muon::String&);
		void freeRegister(const muon::String&);
		void freeRegister(muon::u8);

	private:
		typedef std::unordered_map<muon::String, muon::u8> HashMapRegistry;
		typedef std::unordered_map<muon::String, SymbolTable*> HashMapNamespace;

		HashMapRegistry* m_registers;
		HashMapNamespace* m_namespaces;
		muon::u8 m_registerId;
	};
}

#endif
