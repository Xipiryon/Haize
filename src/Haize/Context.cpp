#include <cstdlib>
#include <sstream>
#include <fstream>

#include <Muon/System/Log.hpp>
#include <Muon/System/Time.hpp>
#include "Haize/VM/OpCode.hpp"

#include "Haize/Engine.hpp"
#include "Haize/Context.hpp"
#include "Haize/Parser/Priv/DefaultCompiler.hpp"

namespace hz
{
	Context::Context(const char* name)
		: m_name(name)
		, m_compiler(NULL)
		//	, m_stack(0)
	{
		//	m_byteCodeModules = MUON_NEW(ByteCodeModuleMap);
		m_error.clear();
	}

	Context::~Context()
	{
		if (m_compiler)
		{
			MUON_DELETE(m_compiler);
		}
		//	MUON_DELETE(m_byteCodeModules);
	}

	//==================================
	//			CONTEXT ATTRIBUTE
	//==================================

	const char* Context::getName() const
	{
		return m_name.cStr();
	}

	Error Context::getLastError() const
	{
		return m_error;
	}

	//==================================
	//			COMPILATION
	//==================================

	void Context::checkCompiler()
	{
		if (!m_compiler)
		{
			m_compiler = MUON_NEW(parser::priv::DefaultCompiler);
		}
	}

	bool Context::load(const m::String& filename)
	{
		checkCompiler();
		m_error.clear();
		m_error.step = Error::LOADING;
		return m_compiler->load(filename, m_error);
	}

	bool Context::compile()
	{
		checkCompiler();
		/*
		m_tokenList = MUON_NEW(std::vector<parser::Token>);
		m_nodeRoot = MUON_NEW(parser::ASTNode);

		// avoid a macro, and avoid duplicating code
		auto clearVariable = [&]()
		{
		m_loadBuffer.clear();
		MUON_DELETE(m_tokenList);
		MUON_DELETE(m_nodeRoot);
		};

		m_error.step = Error::COMPILATION;
		if (!parseLexical())
		{
		clearVariable();
		return false;
		}
		m_loadBuffer.clear();

		if (!parseSyntaxic())
		{
		clearVariable();
		return false;
		}
		m_tokenList->clear();

		if (!parseSemantic())
		{
		clearVariable();
		return false;
		}

		clearVariable();
		//*/
		return true;
	}

	//==================================
	//			EXECUTION FLOW
	//==================================

	bool Context::prepare(const m::String& func)
	{
		return false;
	}

	bool Context::execute()
	{
		return false;
	}

	//==================================
	//			EXECUTION
	//==================================

	bool Context::eval(const char* code, bool storeCode /* = false*/)
	{
		m_error.step = Error::COMPILATION;

		return false;
	}

	bool Context::run(const ByteCode* buffer)
	{
		m_error.step = Error::EXECUTION;

		return false;
	}
}
