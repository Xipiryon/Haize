#ifndef _HAIZE_SYMBOLTABLE_INCLUDE
#define _HAIZE_SYMBOLTABLE_INCLUDE

#include <unordered_map>
#include <Muon/Type/String.hpp>
#include "Haize/Core/Define.hpp"

namespace hz
{
	class HAIZE_API SymbolTable
	{
	public:
		SymbolTable();
		~SymbolTable();

		u8 checkOrCreateRegister(const muon::String&);
		void freeRegister(const muon::String&);
		void freeRegister(u8);

	private:
		typedef std::unordered_map<muon::String, u8> HashMapRegistry;
		typedef std::unordered_map<muon::String, SymbolTable*> HashMapNamespace;

		HashMapRegistry* _registers;
		HashMapNamespace* _namespaces;
		u8 _registerId;
	};
}

#endif
