#include <cstdlib>
#include <sstream>
#include <fstream>

#include <Muon/System/Log.hpp>
#include <Muon/System/Time.hpp>
#include "Haize/VM/OpCode.hpp"

#include "Haize/Engine.hpp"
#include "Haize/Context.hpp"

namespace
{
	void arithmetic(hz::Context&, hz::eOpCode, m::u32, m::u32, m::u32);
	//void printError(hz::parser::InfoError& error);
}

namespace hz
{
	Context::Context(const char* name)
		: m_name(name)
		//	, m_stack(0)
	{
		//	m_byteCodeModules = MUON_NEW(ByteCodeModuleMap);
		clearError(true);
	}

	Context::~Context()
	{
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

	void Context::clearError(bool cleared)
	{
		m_error.message.clear();
		m_error.section.clear();
		m_error.function.clear();
		m_error.line = 0;
		m_error.column = 0;
	}

	void Context::tokenError(const parser::Token& token, const m::String& err)
	{
		clearError(false);
		m_error.line = token.line;
		m_error.column = token.column;
		m_error.message = err;
	}

	//==================================
	//			COMPILATION
	//==================================

	bool Context::load(const m::String& filename)
	{
		m_error.step = Error::LOADING;
		std::ifstream file(filename.cStr());
		if (!file)
		{
			clearError(false);
			m_error.message = "Couldn't open the file!";
			return false;
		}

		if (!file.eof())
		{
			// Get stream size
			file.seekg(0, file.end);
			m::u64 length = (m::u64)file.tellg();
			file.seekg(0, file.beg);

			char* buffer = (char*)malloc((m::u32)length + 1);
			file.read(buffer, length);
			buffer[file.gcount()] = 0;
			m_loadBuffer += buffer;
			free(buffer);

			clearError(true);
			return true;
		}
		clearError(false);
		m_error.message = "File stream seems to be empty: encountered EOF at beginning!";
		return false;
	}

	bool Context::compile()
	{
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
		return true;
	}

	//==================================
	//			EXECUTION FLOW
	//==================================

	bool Context::prepare(const m::String& func)
	{
		m_error.step = Error::PREPARATION;
		return false;
	}

	bool Context::execute()
	{
		m_error.step = Error::EXECUTION;
		/*
		auto it = m_byteCodeModules->find(module);
		if(it != m_byteCodeModules->end())
		{
		return run(it->second);
		}
		//*/
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
