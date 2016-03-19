#ifndef INCLUDE_HAIZE_DEFAULTCOMPILER_INCLUDED
#define INCLUDE_HAIZE_DEFAULTCOMPILER_INCLUDED

#include "Haize/Error.hpp"
#include "Haize/Parser/ASTNode.hpp"

namespace hz
{
	namespace parser
	{
		class HAIZE_API Compiler
		{
		public:
			virtual bool load(const char* buffer, Error& error);
			virtual bool load(const m::String& file, Error& error);

			virtual bool compile(Error& error);

		private:
			// Variables
			m::String m_loadBuffer;						// Load
			std::vector<parser::Token>* m_tokenList;	// Lexical (Lex)
			parser::ASTNode* m_nodeRoot;				// Syntaxic (Lemon)
			//SymbolTable m_symbols;					// Semantic
		};
	}
}
#endif
