#ifndef INCLUDE_HAIZE_DEFAULTCOMPILER_INCLUDED
#define INCLUDE_HAIZE_DEFAULTCOMPILER_INCLUDED

#include "Haize/Error.hpp"
#include "Haize/Parser/ASTNode.hpp"

namespace hz
{
	namespace parser
	{
		namespace internal
		{
			struct ISharedData
			{
				virtual ~ISharedData()
				{
				}
			};
		}

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
			void pushToken(internal::ISharedData*, m::String&);
			void pushSeparatorToken(internal::ISharedData*, m::String&);
			// Syntaxic
			void tokenError(Error& err, const parser::Token&, const m::String&);
			bool readToken(internal::ISharedData*, parser::Token&, m::u32);
			bool popToken(internal::ISharedData*, m::u32);

			bool parseExpression(internal::ISharedData*);
			bool parseGlobal(internal::ISharedData*);
			bool parseNamespace(internal::ISharedData*);
			bool parseClass(internal::ISharedData*);
			bool parseFunction(internal::ISharedData*);
			// Semantic
		};
	}
}
#endif
