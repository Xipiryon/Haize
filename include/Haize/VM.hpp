
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
		bool load(const std::istream& str);

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
			return _info;
		}

	private:
		parser::Info _info;

		ByteCode _instr;
		muon::u32 _stack;
		SymbolTable _symbols;
		muon::Variant _registers[ByteCode::INVALID_REG];
	};
}
#endif

