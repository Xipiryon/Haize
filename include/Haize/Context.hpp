
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
		enum eStep
		{
			LOADING,
			COMPILATION,
			PREPARATION,
			EXECUTION,
		} step;

		m::String message;
		m::String section;
		m::String function;
		m::u32 line;
		m::u32 column;

		bool _cleared;
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
		bool load(std::istream& stream);

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
		InfoError m_error;

		// PARSER & COMPILATION
		// ****************************
		void clearError(bool);
		void tokenError(const parser::Token&, const m::String&);
		bool parseLexical();
		bool parseSyntaxic();
		bool parseSemantic();
		// Variables
		m::String m_loadBuffer;
		std::vector<parser::Token>* m_tokenList;
		parser::ASTNode* m_nodeRoot;
		SymbolTable m_symbols;
		// Lexical
		void pushToken(parser::InfoImpl*, m::String&);
		void pushSeparatorToken(parser::InfoImpl*, m::String&);
		// Syntaxic
		bool readToken(parser::Token&, bool);
		bool parseChunk(parser::InfoImpl*);
		bool parseExpression(parser::InfoImpl*);
		bool parseGlobal(parser::InfoImpl*);
		bool parseNamespace(parser::InfoImpl*);
		bool parseClass(parser::InfoImpl*);
		bool parseFunction(parser::InfoImpl*);
		// Semantic

		// EXECUTION
		// ****************************
		ByteCode m_instr;
		m::u32 m_stack;
		m::Variant m_registers[ByteCode::REG_MAX_AVAILABLE];
	};

}
#endif

