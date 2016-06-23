#ifndef INCLUDE_HAIZE_DEFAULTCOMPILER_INCLUDED
#define INCLUDE_HAIZE_DEFAULTCOMPILER_INCLUDED

#include "Haize/Error.hpp"
#include "Haize/Compiler/Token.hpp"
#include "Haize/Compiler/ASTNode.hpp"
#include "Haize/Compiler/DeclStruct.hpp"
#include "Haize/Runtime/ByteCode.hpp"
#include "Haize/Runtime/Variant.hpp"

namespace hz
{
	class HAIZE_API Module
	{
	public:

		struct Section
		{
			m::String name;
			m::u64 _end;
		};

		Module(const char* name, bool loadDefaultObjects = true);
		Module(const m::String& name, bool loadDefaultObjects = true);
		~Module();

		const m::String& getName() const;

		bool load(const m::String& file);
		bool compile();
		Error getError() const;

		m::u32 getByteCodeSize() const;
		ByteCode* getByteCodePtr();

		template<typename Ret, typename...Args>
		bool registerFunction(m::String name, Ret(*function)(Args...));

	protected:
		bool load(const m::String& file, Error& error);
		bool lexical(Error& error);
		bool syntaxic(Error& error);
		bool semantic(Error& error);

	private:
		void registerDefaultObjects();

		// Module Information
		m::String m_name;

		// Load & Compilation info
		Error m_error;

		m::String m_loadBuffer;
		std::vector<parser::Token>* m_tokenList;
		std::vector<Section>* m_sections;
		parser::ASTNode* m_nodeRoot;

		// Symbol Table
		symbol::Namespace m_rootNamespace;
		std::vector<ByteCode>* m_byteCode;
	};

	template<typename Ret, typename...Args>
	bool registerFunction(m::String name, Ret(*function)(Args...))
	{
		return true;
	}
}
#endif
