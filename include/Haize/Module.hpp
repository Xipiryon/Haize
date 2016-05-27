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
	class HAIZE_API ModuleMemConfig
	{
	public:
		ModuleMemConfig();
	private:
	};

	class HAIZE_API Module
	{
	public:
		struct Section
		{
			m::String name;
			m::u64 _end;
		};

		Module(const char* name, ModuleMemConfig memConfig = ModuleMemConfig());
		Module(const m::String& name, ModuleMemConfig memConfig = ModuleMemConfig());
		~Module();

		const m::String& getName() const;

		bool load(const m::String& file);
		bool compile();
		Error getLastError() const;

#if defined(HAIZE_DEBUG)
		m::u32 getByteCodeSize() const
		{
			return m_byteCode.size() * sizeof(ByteCode);
		}

		ByteCode* getByteCodePtr()
		{
			return (m_byteCode.empty() ? NULL : &m_byteCode[0]);
		}
#endif
	protected:
		bool load(const m::String& file, Error& error);
		bool lexical(Error& error);
		bool syntaxic(Error& error);
		bool semantic(Error& error);

	private:

		// Module Information
		m::String m_name;

		// Load & Compilation info
		Error m_error;
		std::vector<ByteCode> m_byteCode;

		m::String m_loadBuffer;
		std::vector<parser::Token>* m_tokenList;
		std::vector<Section>* m_sections;
		parser::ASTNode* m_nodeRoot;
		std::vector<ByteCode> m_bytecode;

		// Symbol Table
		symbol::Namespace m_rootNamespace;
	};
}
#endif
