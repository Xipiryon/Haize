
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
		* @brief Read the content of the sstream
		* @param stream Stream to read
		* @note Only one file can be loaded at a time
		*/
		bool load(std::istream& stream);

		/*!
		* @brief Load a file located at path
		* @param file File to load
		* @note Only one file can be loaded at a time
		*/
		bool load(const char* file);

		/*!
		*
		*/
		bool compile(const muon::String& module);

		/*!
		*
		*/
		bool compile(const char* module);

		/*!
		*
		*/
		bool execute(const muon::String& module);

		/*!
		*
		*/
		bool execute(const char* module);

		/*!
		*
		*/
		bool eval(const char* str);

		/*!
		*
		*/
		bool run(const ByteCode* instr);

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
		std::map<muon::String, ByteCode*>* m_byteCodeModules;
	};
}
#endif

