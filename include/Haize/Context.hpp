
#ifndef INCLUDE_HAIZE_CONTEXT_INCLUDED
#define INCLUDE_HAIZE_CONTEXT_INCLUDED

#include <vector>

#include <Muon/Variant.hpp>
#include "Haize/VM/ByteCode.hpp"
#include "Haize/VM/SymbolTable.hpp"

#include "Haize/Parser/ASTNode.hpp"

namespace hz
{
	namespace parser
	{
		struct InfoImpl
		{
			virtual ~InfoImpl()
			{
			}
		};
	}

	class Engine;

	/*!
	*
	*/
	struct HAIZE_API InfoError
	{
		muon::String message;
		muon::String section;
		muon::String function;
		muon::u32 line;
		muon::u32 column;
	};

	/*!
	*
	*/
	enum eLoadState
	{
		LOAD_SUCCESS,		//!< Everything went good
		LOAD_ERROR,			//!< Generic error message
		LOAD_EMPTY_BUFFER,	//!< Buffer or loaded file is empty
	};

	/*!
	*
	*/
	enum eCompilationState
	{
		COMPILATION_SUCCESS,					//!< Everything went good
		COMPILATION_ERROR_LEXICAL_NUMBER,		//!< An encountered number is illformed
		COMPILATION_ERROR_LEXICAL_IDENTIFIER,	//!< An encountered identifier is illformed
		COMPILATION_ERROR_LEXICAL_OPERATOR,		//!< An encountered operator is illformed
		COMPILATION_ERROR_SYNTAXIC,				// TODO
		COMPILATION_ERROR_SYNTAXIC_FREE_CODE,	//!< Language doesn't allow free code, which as been found somewhere
		COMPILATION_ERROR_SEMANTIC,				// TODO
		COMPILATION_ERROR,						//!< Generic error message
	};

	/*!
	*
	*/
	enum ePreparationState
	{
		PREPARATION_SUCCESS,	//!< Everything went good
		PREPARATION_ERROR,		//!< Generic error message
	};

	/*!
	*
	*/
	enum eExecutationState
	{
		EXECUTION_SUCCESS,		//!< Everything went good
		EXECUTION_ERROR,		//!< Generic error message
	};

	static const char* s_eLoadStateStr[] =
	{
		"LOAD_SUCCESS",
		"LOAD_ERROR",
		"LOAD_EMPTY_BUFFER",
	};

	static const char* s_eCompilationStateStr[] =
	{
		"COMPILATION_SUCCESS",
		"COMPILATION_ERROR_LEXICAL_NUMBER",
		"COMPILATION_ERROR_LEXICAL_IDENTIFIER",
		"COMPILATION_ERROR_LEXICAL_OPERATOR",
		"COMPILATION_ERROR_SYNTAXIC",
		"COMPILATION_ERROR_SEMANTIC",
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
		* @brief Return last error message
		* If an error occured and you want more information than just the returned enum,
		* this function will return a hz::InfoError struct containing details.
		* @note The struct will be reset on any succesful function call
		*/
		InfoError getLastError() const;

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
		eCompilationState parseLexical();
		eCompilationState parseSyntaxic();
		eCompilationState parseSemantic();

		muon::String m_name;
		InfoError m_error;

		// PARSER & COMPILATION
		// ****************************
		// Variables
		muon::String m_loadBuffer;
		std::vector<parser::Token>* m_tokenList;
		parser::ASTNode* m_nodeRoot;
		SymbolTable m_symbols;

		// Lexical
		void pushToken(parser::InfoImpl*, muon::String&);
		void pushSeparatorToken(parser::InfoImpl*, muon::String&);
		// Syntaxic
		void parseExpression(parser::InfoImpl*);
		void parseGlobal(parser::InfoImpl*);
		void parseClass(parser::InfoImpl*);
		void parseFunction(parser::InfoImpl*);
		// Semantic

		// EXECUTION
		// ****************************
		ByteCode m_instr;
		muon::u32 m_stack;
		muon::Variant m_registers[ByteCode::REG_MAX_AVAILABLE];
	};

}
#endif

