
#ifndef _HAIZE_VIRTUALMACHINE_INCLUDED
#define _HAIZE_VIRTUALMACHINE_INCLUDED

#include <unordered_map>
#include <Muon/Meta/Variant.hpp>
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
		muon::meta::Variant execute(const ByteCode* instr);

		/*!
		*
		*/
		muon::meta::Variant execute();

		/*!
		*
		*/
		muon::meta::Variant eval(const char* str);

		const parser::Info& getInfo() const
		{
			return _info;
		}

	private:
		parser::Info _info;

		ByteCode _instr;
		muon::u32 _stack;
		SymbolTable _symbols;
		muon::meta::Variant _registers[ByteCode::INVALID_REG];
	};
}
#endif

