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
			Compiler();
			~Compiler();

			bool load(const char* buffer, Error& error);
			bool load(const m::String& file, Error& error);

			bool compile(Error& error);

		private:
			// Variables
			m::String m_loadBuffer;
			parser::ASTNode* m_nodeRoot;
			//SymbolTable m_symbols;
		};
	}
}
#endif
