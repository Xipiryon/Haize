#ifndef INCLUDE_HAIZE_DEFAULTCOMPILER_INCLUDED
#define INCLUDE_HAIZE_DEFAULTCOMPILER_INCLUDED

#include "Haize/Error.hpp"
#include "Haize/Parser/ICompiler.hpp"
#include "Haize/Parser/ASTNode.hpp"

namespace hz
{
	namespace parser
	{
		namespace priv
		{
			struct HAIZE_API SharedData
			{
				virtual ~SharedData()
				{
				}
			};

			class HAIZE_API DefaultCompiler : public ICompiler
			{
			public:
				virtual bool load(const char* buffer, Error& error);
				virtual bool load(const m::String& file, Error& error);

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
				void pushToken(SharedData*, m::String&);
				void pushSeparatorToken(SharedData*, m::String&);
				// Syntaxic
				void tokenError(Error& err, const parser::Token&, const m::String&);
				bool readToken(parser::Token&, m::u32);
				bool popToken(m::u32);

				bool parseExpression(SharedData*);
				bool parseGlobal(SharedData*);
				bool parseNamespace(SharedData*);
				bool parseClass(SharedData*);
				bool parseFunction(SharedData*);
				// Semantic
			};
		}
	}
}
#endif
