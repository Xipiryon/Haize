
#ifndef _HAIZE_VIRTUALMACHINE_INCLUDED
#define _HAIZE_VIRTUALMACHINE_INCLUDED

#include <unordered_map>
#include <Muon/Variant.hpp>
#include "Haize/Parser/Info.hpp"
#include "Haize/VM/ByteCode.hpp"
#include "Haize/VM/SymbolTable.hpp"

/*!
*
*/
namespace hz
{
	/*!
	*
	*/
	class HAIZE_API Context
	{
	};

	/*!
	*
	*/
	class HAIZE_API VMInstance
	{
	public:
		VMInstance();
		~VMInstance();

		/*!
		*
		*/
		bool load(std::istream& file);

		/*!
		*
		*/
		bool load(const char* str);

		/*!
		*
		*/
		bool compile(const muon::String& module);

		/*!
		*
		*/
		bool execute(const ByteCode* instr);

		/*!
		*
		*/
		bool execute();

		/*!
		*
		*/
		bool eval(const char* str);

		const parser::Info& getInfo() const
		{
			return m_info;
		}

	private:
		parser::Info m_info;

		ByteCode m_instr;
		muon::u32 m_stack;
		SymbolTable m_symbols;
		muon::Variant m_registers[ByteCode::INVALID_REG];

		char* m_loadBuffer;
	};
}
#endif

