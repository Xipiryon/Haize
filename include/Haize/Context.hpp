#ifndef INCLUDE_HAIZE_CONTEXT_INCLUDED
#define INCLUDE_HAIZE_CONTEXT_INCLUDED

#include <vector>

#include <Muon/Variant.hpp>
#include "Haize/VM/ByteCode.hpp"
#include "Haize/VM/SymbolTable.hpp"

#include "Haize/Parser/Compiler.hpp"
#include "Haize/Parser/ASTNode.hpp"

namespace hz
{
	class Engine;

	/*!
	* @brief Script execution entry point
	* Context are used both for compilation and execution.
	* The whole process follow the given order
	* 1. Loading files
	*	-# There is no "include" in the language
	*	-# If 'A.hz' requires 'B.hz', you will have to load both
	* 2. Compilation
	*	-# When every files are loaded, compilation will translates text code
	*		into intermediate representation
	*	-# If 'A.hz' requires 'B.hz', but 'B.hz' is not loaded, compiling 'A.hz' will fail
	* 3. Preparation
	*	-# Set the variable you're going to call
	*	-# Push variables in the Context
	* 4. Execution
	*	-# Start executing intermediate code for the prepared function
	*/
	class HAIZE_API Context
	{
		friend class Engine;
	public:
		Context(const m::String& name);
		~Context();

		/*!
		* @brief Return the Context name
		* @return Context name
		*/
		const char* getName() const;

		/*!
		* @brief Return last error message
		* If an error occured and you want more information than just the returned enum,
		* this function will return a hz::InfoError struct containing details.
		* @note The struct will be reset on any succesful function call
		*/
		Error getLastError() const;

		/*!
		* @brief Load a file located at path
		* @param file File to load
		*/
		bool load(const m::String& file);

		/*!
		* @brief
		*/
		bool compile();

		/*!
		* @brief Prepare a function to call
		* Only function can be executed, there is no "free code"
		* like in Lua or Python, a file that contains only code will not
		* be loaded
		* @param func Function name to be loaded, with prefixed namespace (ex: "namespace::function")
		*/
		bool prepare(const m::String& func);

		/*!
		* @brief Start executing the prepared function
		*
		*/
		bool execute();

		/*!
		* @brief Compile and execute code
		* Load, compile and execute a string containing code.
		* By default, if there is any struct or function declaration,
		* it is not added to the current Context.
		*
		* @param str Buffer of code to be loaded, compiled and executed
		* @param storeCode Make the context store any new function / struct (Disabled by default)
		* @return False if any of the loading, compiling, execution step failed  (see ::getLastError())
		*/
		bool eval(const char* str, bool storeCode = false);

		/*!
		* @brief Execute ByteCode directly
		* There is no check made upon bytecode execution,
		* like checking if functions or structs are defined
		*
		* @param instr ByteCode buffer to be executed
		*/
		bool run(const ByteCode* instr);

	private:
		m::String m_name;
		Error m_error;

		parser::Compiler m_compiler;
		// EXECUTION
		// ****************************
		ByteCode m_instr;
		m::u32 m_stack;
		m::Variant m_registers[ByteCode::REG_MAX_AVAILABLE];
	};
}
#endif
