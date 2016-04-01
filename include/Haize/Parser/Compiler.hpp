#ifndef INCLUDE_HAIZE_DEFAULTCOMPILER_INCLUDED
#define INCLUDE_HAIZE_DEFAULTCOMPILER_INCLUDED

#include "Haize/Error.hpp"
#include "Haize/Parser/Token.hpp"
#include "Haize/Parser/ASTNode.hpp"

namespace hz
{
	namespace parser
	{
		struct InternalCompilerData
		{
		};

		class HAIZE_API Compiler
		{
		public:
			virtual bool load(const char* buffer, Error& error);
			virtual bool load(const m::String& file, Error& error);

			virtual bool compile(Error& error);

		protected:
			virtual bool lexical(Error& error);
			virtual bool syntaxic(Error& error);
			virtual bool semantic(Error& error);

		private:
			// Variables
			m::String m_loadBuffer;
			std::vector<parser::Token>* m_tokenList;
			parser::ASTNode* m_nodeRoot;
			//SymbolTable m_symbols;

			// Lexical
			void pushToken(InternalCompilerData*, m::String&);
			void pushSeparatorToken(InternalCompilerData*, m::String&);

			// Syntaxic
			void tokenError(Error& err, const parser::Token&, const m::String&);
			bool readToken(InternalCompilerData*, parser::Token&, m::u32);
			bool popToken(InternalCompilerData*, m::u32);

			bool parseExpression(InternalCompilerData*);
			bool parseGlobal(InternalCompilerData*);
			bool parseNamespace(InternalCompilerData*);
			bool parseClass(InternalCompilerData*);
			bool parseFunction(InternalCompilerData*);
			// Semantic
		};
	}
}
#endif
