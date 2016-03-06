
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

	InfoError Context::getLastError() const
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
		m_error._cleared = cleared;
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

	bool Context::load(std::istream& file)
	{
		m_error.step = InfoError::LOADING;
		if (file)
		{
			if (!file.eof())
			{
				// Get stream size
				file.seekg(0, file.end);
				m::u64 length = (m::u64)file.tellg();
				file.seekg(0, file.beg);

				char* buffer = (char*)malloc((m::u32)length);
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
		clearError(false);
		m_error.message = "Couldn't open the file stream!";
		return false;
	}

	bool Context::load(const m::String& filename)
	{
		m_error.step = InfoError::LOADING;
		std::ifstream file(filename.cStr());
		if(!file)
		{
			clearError(false);
			m_error.message = "Couldn't open the file!";
			return false;
		}
		return load(file);
	}

	bool Context::compile()
	{
		if(!m_error._cleared)
		{
			return false;
		}

		m_tokenList = MUON_NEW(std::vector<parser::Token>);
		m_nodeRoot = MUON_NEW(parser::ASTNode);

		// avoid a macro, and avoid duplicating code
		auto clearVariable = [&]()
		{
			m_loadBuffer.clear();
			MUON_DELETE(m_tokenList);
			MUON_DELETE(m_nodeRoot);
		};

		m_error.step = InfoError::COMPILATION;
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

		if(!parseSemantic())
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
		if(!m_error._cleared)
		{
			return false;
		}

		m_error.step = InfoError::PREPARATION;
		return false;
	}

	bool Context::execute()
	{
		if(!m_error._cleared)
		{
			return false;
		}

		m_error.step = InfoError::EXECUTION;
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
		m_error.step = InfoError::COMPILATION;
		/*
		m::system::Log log("VM", m::LOG_DEBUG);
		m::Variant nil;
		m_info.error.section = "#RunTimeEval#";

#if defined(MUON_DEBUG)
		m::system::Time time;
		m::f32 totalTime = 0;

		// Lexical
		time.start();
		{
			if (!hz::parser::lexical::parse(m_info, code))
			{
				printError(m_info.error);
				return false;
			}
			hz::parser::lexical::display(m_info);
		}
		totalTime += time.now();
		log() << "Lexical parse: " << time.now() << " seconds" << m::endl;

		// Syntaxic
		time.start();
		{
			if (!hz::parser::syntaxic::parse(m_info))
			{
				printError(m_info.error);
				return false;
			}
			hz::parser::syntaxic::display(m_info);
		}
		totalTime += time.now();
		log() << "Syntaxic parse: " << time.now() << " seconds" << m::endl;

		// Semantic
		time.start();
		{
			if (!hz::parser::semantic::parse(m_info))
			{
				printError(m_info.error);
				return false;
			}
			hz::parser::semantic::display(m_info);
		}
		totalTime += time.now();
		log() << "ByteCode Creation: " << time.now() << " seconds" << m::endl;
		log() << "** Total Parse Time: " << totalTime << " seconds **" << m::endl;
#else
		if(!(hz::parser::lexical::parse(m_info, code)
			&& hz::parser::syntaxic::parse(m_info)
			&& hz::parser::semantic::parse(m_info)))
		{
			printError(m_info.error);
			return false;
		}
#endif
		// Execution
		return run(m_info.IRCode);
		//*/
		return false;
	}

	bool Context::run(const ByteCode* buffer)
	{
		m_error.step = InfoError::EXECUTION;
		/*
		m::system::Log log("VM");
#ifdef MUON_DEBUG
#	define DEBUG_CASE(Case) case Case:
		log(m::LOG_DEBUG) << "** Execute Debug Log enabled **" << m::endl;
#else
#	define DEBUG_CASE(Case) case Case:
#endif
		m_stack = 0;
		m::u32 reg[3] = { 0, 0, 0 };

		bool exec = true;
		do
		{
			m_instr = *(buffer + m_stack);
			eOpCode opCode = m_instr.opCode();
			switch (opCode)
			{
				DEBUG_CASE(SYS_MOV)
				{
					//(*registers)[instr.res()] = (*registers)[instr.argG()];
					break;
				}

				DEBUG_CASE(SYS_SETRAW)
				{
					m::u64 type = m_instr.argG();
					m::u8 dest = m_instr.res();
					m::RawPointer rawData = m::RawPointer(buffer + m_stack + 1);
					//_registers[dest].set(rawData);
					m_stack += 2;
					break;
				}

				// *****
				DEBUG_CASE(MATH_ADD)
				DEBUG_CASE(MATH_SUB)
				DEBUG_CASE(MATH_MUL)
				DEBUG_CASE(MATH_DIV)
				{
					arithmetic(*this, opCode, m_instr.res(), m_instr.arg1(), m_instr.arg2());
					break;
				}
				DEBUG_CASE(MATH_MOD)
				{
					break;
				}
				DEBUG_CASE(MATH_ASN)
				{
					m_registers[m_instr.res()] = m_registers[m_instr.argG()];
					break;
				}

				// *****
				DEBUG_CASE(BIT_LSH)
				{
					break;
				}
				DEBUG_CASE(BIT_RSH)
				{
					break;
				}

				// *****
				DEBUG_CASE(BIT_AND)
				{
					break;
				}
				DEBUG_CASE(BIT_OR)
				{
					break;
				}
				DEBUG_CASE(BIT_XOR)
				{
					break;
				}
				DEBUG_CASE(BIT_NOT)
				{
					break;
				}

				// *****
				DEBUG_CASE(CMP_NOT)
				{
					break;
				}
				DEBUG_CASE(CMP_LT)
				{
					break;
				}
				DEBUG_CASE(CMP_LET)
				{
					break;
				}
				DEBUG_CASE(CMP_EQU)
				{
					break;
				}
				DEBUG_CASE(CMP_AND)
				{
					break;
				}
				DEBUG_CASE(CMP_OR)
				{
					break;
				}

				// *****
				DEBUG_CASE(SYS_JUMP)
				{
					break;
				}
				DEBUG_CASE(SYS_RETURN)
				{
					m_registers[m_instr.res()] = m_registers[m_instr.argG()];
					break;
				}
				DEBUG_CASE(SYS_CALL)
				{
					break;
				}

				// *****
				DEBUG_CASE(SYS_PRG_END)
				{
					exec = false;
					break;
				}

				// *****
				default:
				{
					log(m::LOG_ERROR) << "Nothing to do for OpCode: " << opCode << m::endl;
				}
			}
			++m_stack;
		} while (exec);
		//*/
		return false;
	}
}

namespace
{
#define VM_REG(Id) (*registers)[Id]
	void arithmetic(hz::Context& vm, hz::eOpCode op, m::u32 rA, m::u32 rB, m::u32 rC)
	{
		/*
		m::f64 left = hz::boxed::getFloating(REG(rB));
		m::f64 right = hz::boxed::getFloating(REG(rC));
		m::f64 r = 0;
		switch (op)
		{
			DEBUG_CASE(hz::OP_ADD) r = left + right; break;
			DEBUG_CASE(hz::OP_SUB) r = left - right; break;
			DEBUG_CASE(hz::OP_MUL) r = left * right; break;
			DEBUG_CASE(hz::OP_DIV) r = left / right; break;
		}
		hz::boxed::setFloating((*registers)[rA], r);
		//*/
	}
#undef VM_REG
	/*
	void printError(hz::parser::InfoError& error)
	{
		m::system::Log log("VM", m::LOG_ERROR);
		log() << "Section: " << error.section << m::endl;
		log() << "Function: " << error.function << " @ " << error.line << ":" << error.column << m::endl;
		log() << "Message: \"" << error.message << "\"" << m::endl;
	}
	//*/
}
