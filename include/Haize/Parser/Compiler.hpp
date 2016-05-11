#ifndef INCLUDE_HAIZE_DEFAULTCOMPILER_INCLUDED
#define INCLUDE_HAIZE_DEFAULTCOMPILER_INCLUDED

#include "Haize/Error.hpp"
#include "Haize/Parser/Token.hpp"
#include "Haize/Parser/ASTNode.hpp"
#include "Haize/Parser/DeclStruct.hpp"
#include "Haize/Interpreter/ByteCode.hpp"

namespace hz
{
	class ContextMemConfig;
	namespace parser
	{
		class HAIZE_API Compiler
		{
		public:
			Compiler();
			~Compiler();

			struct Section
			{
				m::String name;
				m::u64 _end;
			};

			bool load(const char* buffer, Error& error);
			bool load(const m::String& file, Error& error);

			bool compile(Error& error, std::vector<ByteCode>* byteCode);

		protected:
			bool lexical(Error& error);
			bool syntaxic(Error& error);
			bool semantic(Error& error);

		private:
			// Variables
			m::String m_loadBuffer;
			std::vector<parser::Token>* m_tokenList;
			std::vector<Section>* m_sections;
			parser::ASTNode* m_nodeRoot;
			parser::DeclNamespace m_rootNamespace;
			std::vector<ByteCode>* m_bytecode;
		};
	}
}
#endif
