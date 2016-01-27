
#ifndef _HAIZE_ENGINE_INCLUDED
#define _HAIZE_ENGINE_INCLUDED

#include <unordered_map>
#include <Muon/Variant.hpp>
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
	public:

	private:
		ByteCode m_instr;
		muon::u32 m_stack;
		SymbolTable m_symbols;
		muon::Variant m_registers[ByteCode::REG_MAX_AVAILABLE];

		char* m_loadBuffer;
	};

	/*!
	*
	*/
	class HAIZE_API Engine
	{
	public:
		Engine();
		~Engine();

		/*!
		* @brief Read the content of the sstream
		* @param stream Stream to read
		* @note Only one file can be loaded at a time
		*/
		bool load(const char* module, std::istream& stream);

		/*!
		* @brief Load a file located at path
		* @param file File to load
		* @note Only one file can be loaded at a time
		*/
		bool load(const char* module, const char* file);

		/*!
		*
		*/
		bool compile(const char* module);

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

	private:

		typedef std::map<muon::String, Context> ModuleContextMap;

		ModuleContextMap m_moduleContext;
	};
}
#endif

