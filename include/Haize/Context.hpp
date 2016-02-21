
#ifndef INCLUDE_HAIZE_CONTEXT_INCLUDED
#define INCLUDE_HAIZE_CONTEXT_INCLUDED

#include <Muon/Variant.hpp>
#include "Haize/VM/ByteCode.hpp"
#include "Haize/VM/SymbolTable.hpp"

namespace hz
{
	class Engine;

	/*!
	*
	*/
	enum eLoadState
	{
		LOAD_SUCCESS,
		LOAD_ERROR,
	};

	/*!
	*
	*/
	enum eCompilationState
	{
		COMPILATION_SUCCESS,
		COMPILATION_ERROR,
	};

	/*!
	*
	*/
	enum ePreparationState
	{
		PREPARATION_SUCCESS,
		PREPARATION_ERROR,
	};

	/*!
	*
	*/
	enum eExecutationState
	{
		EXECUTION_SUCCESS,
		EXECUTION_ERROR,
	};

	static const char* s_eLoadStateStr[] =
	{
		"LOAD_SUCCESS",
		"LOAD_ERROR",
	};

	static const char* s_eCompilationStateStr[] =
	{
		"COMPILATION_SUCCESS",
		"COMPILATION_ERROR",
	};

	static const char* s_ePreparationStateStr[] =
	{
		"PREPARATION_SUCCESS",
		"PREPARATION_ERROR",
	};

	static const char* s_eExecutationStateStr[] =
	{
		"EXECUTION_SUCCESS",
		"EXECUTION_ERROR",
	};

	/*!
	* @brief Script execution entry point
	* Context are used both for compilation and execution.
	* The whole process follow the given order
	* 1. Loading files
	*	-# There is no "include" in the language
	*	-# If 'A.hz' requires 'B.hz', you will have to load both (order doesn't matter)
	* 2. Compilation
	*	-# When every files are loaded, compilation will translates text code
	*		into intermediate representation
	*	-# If 'A.hz' requires 'B.hz', but 'B.hz' is not (at least) loaded, compiling 'A.hz' will fail
	* 3. Preparation
	*	-# Push variables / functions on the stack
	*	-# Indicate the entry point of the Context
	* 4. Execution
	*	-# Start executing intermediate code
	*/
	class HAIZE_API Context
	{
		friend class Engine;
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
		*/
		eLoadState load(std::istream& stream);

		/*!
		* @brief Load a file located at path
		* @param file File to load
		*/
		eLoadState load(const muon::String& file);

		/*!
		* @brief 
		*/
		eCompilationState compile();

		/*!
		* @brief Prepare a function to call
		* Only function can be executed, there is no "free code" 
		* like in Lua or Python, a file that contains only code will not
		* be loaded
		* @param func Function name to be loaded, with prefixed namespace (ex: "namespace::function")
		*/
		ePreparationState prepare(const muon::String& func);

		/*!
		* @brief Start executing the prepared function
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

		eLoadState m_lastLoadState;
		eCompilationState m_lastCompilationState;
		ePreparationState m_lastPreparationState;
		eExecutationState m_lastExecutionState;

		ByteCode m_instr;
		muon::u32 m_stack;
		SymbolTable m_symbols;
		muon::Variant m_registers[ByteCode::REG_MAX_AVAILABLE];

		char* m_loadBuffer;
	};

}
#endif

