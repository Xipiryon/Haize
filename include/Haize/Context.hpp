
#ifndef INCLUDE_HAIZE_CONTEXT_INCLUDED
#define INCLUDE_HAIZE_CONTEXT_INCLUDED

#include <Muon/Variant.hpp>
#include "Haize/VM/ByteCode.hpp"
#include "Haize/VM/SymbolTable.hpp"

namespace hz
{
	class Engine;

	enum eLoadState
	{
		LOAD_SUCCESS,
		LOAD_ERROR,
	};

	enum eCompilationState
	{
		COMPILATION_SUCCESS,
		COMPILATION_ERROR,
	};

	enum eExecutationState
	{
		EXECUTION_SUCCESS,
		EXECUTION_ERROR,
	};

	/*!
	* @brief Script execution entry point
	* Context are used both for compilation and execution.
	*/
	class HAIZE_API Context
	{
		friend class Engine;
		friend class muon::memory::DefaultAllocator;
		Context(const char* name);
	public:
		~Context();

		/*!
		* @brief Return the Context name
		* @return Context name
		*/
		const char* getName() const;

		/*!
		* @brief Read the content of the sstream
		* @param stream Stream to read
		* @note Only one file can be loaded at a time
		*/
		eLoadState load(std::istream& stream);

		/*!
		* @brief Load a file located at path
		* @param file File to load
		* @note Only one file can be loaded at a time
		*/
		eLoadState load(const char* file);

		/*!
		*
		*/
		eCompilationState compile();

		/*!
		*
		*/
		eExecutationState prepare();

		/*!
		*
		*/
		eExecutationState execute();

		/*!
		* @brief Compile and execute code
		* Load, compile and execute a string containing code.
		* By default, if there is any struct or function declaration,
		* it is not added to the current Context.
		*
		* @param str Buffer of code to be loaded, compiled and executed
		* @param storeCode Make the context store any new function / struct (Disabled by default)
		* @return Execution state, from eExecutationState
		*/
		eExecutationState eval(const char* str, bool storeCode = false);

		/*!
		* @brief Execute ByteCode directly
		* There is no check made upon bytecode execution,
		* like checking if functions or structs are defined
		*
		* @param instr ByteCode buffer to be executed
		* @return Execution state, from eExecutationState
		*/
		eExecutationState run(const ByteCode* instr);

	private:
		muon::String m_name;

		ByteCode m_instr;
		muon::u32 m_stack;
		SymbolTable m_symbols;
		muon::Variant m_registers[ByteCode::REG_MAX_AVAILABLE];

		char* m_loadBuffer;
	};

}
#endif

