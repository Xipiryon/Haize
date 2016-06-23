#include <fstream>
#include <sstream>
#include <Muon/System/Log.hpp>

#include "Haize/Context.hpp"
#include "Haize/Module.hpp"

namespace hz
{
	Module::Module(const char* name, bool loadDefaultObjects)
		: m_name(name)
	{
		m_sections = MUON_NEW(std::vector<Section>);
		m_byteCode = MUON_NEW(std::vector<ByteCode>);
		m_rootNamespace.name = symbol::Namespace::g_GlobalNamespaceName;

		if (loadDefaultObjects)
		{
			registerDefaultObjects();
		}
	}

	Module::Module(const m::String& name, bool loadDefaultObjects)
		: Module(name.cStr())
	{
	}

	Module::~Module()
	{
		MUON_DELETE(m_sections);
		MUON_DELETE(m_byteCode);
	}

	const m::String& Module::getName() const
	{
		return m_name;
	}

	Error Module::getError() const
	{
		return m_error;
	}

	bool Module::load(const m::String& filename)
	{
		m_error.clear();
		m_error.step = Error::LOADING;
		return load(filename, m_error);
	}

	bool Module::compile()
	{
		m_error.clear();
		m_error.step = Error::COMPILATION;
		m_error.state = Error::ERROR;

		m_tokenList = MUON_NEW(std::vector<parser::Token>);
		m_nodeRoot = MUON_NEW(parser::ASTNode);

		// avoid a macro, and avoid duplicating code
		auto clearVariable = [&]()
		{
			m_loadBuffer.clear();
			MUON_DELETE(m_tokenList);
			MUON_DELETE(m_nodeRoot);
		};

		if (!lexical(m_error))
		{
			clearVariable();
			return false;
		}

		if (!syntaxic(m_error))
		{
			clearVariable();
			return false;
		}

		if (!semantic(m_error))
		{
			clearVariable();
			return false;
		}

		clearVariable();
		m_error.state = Error::SUCCESS;
		return true;
	}

	m::u32 Module::getByteCodeSize() const
	{
		return m_byteCode->size() * sizeof(ByteCode);
	}

	ByteCode* Module::getByteCodePtr()
	{
		return (m_byteCode->empty() ? NULL : &(*m_byteCode)[0]);
	}
}
