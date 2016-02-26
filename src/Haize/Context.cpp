
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
	void arithmetic(hz::Context&, hz::eOpCode, muon::u32, muon::u32, muon::u32);
	//void printError(hz::parser::InfoError& error);
}

namespace hz
{
	Context::Context(const char* name)
		: m_name(name)
	//	, m_stack(0)
	{
	//	m_byteCodeModules = MUON_NEW(ByteCodeModuleMap);
		m_tokenList = MUON_NEW(std::vector<parser::Token>);
		m_nodeRoot = MUON_NEW(parser::ASTNode);
	}

	Context::~Context()
	{
		MUON_DELETE(m_tokenList);
		MUON_DELETE(m_nodeRoot);
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

	//==================================
	//			COMPILATION
	//==================================

	eLoadState Context::load(std::istream& file)
	{
		if (file)
		{
			if (!file.eof())
			{
				// Get stream size
				file.seekg(0, file.end);
				muon::u64 length = (muon::u64)file.tellg();
				file.seekg(0, file.beg);
				if(length == 0)
				{
					return LOAD_EMPTY_BUFFER;
				}

				char* buffer = (char*)malloc((muon::u32)length);
				file.read(buffer, length);
				buffer[file.gcount()] = 0;
				m_loadBuffer += buffer;
				free(buffer);

				return LOAD_SUCCESS;
			}
			return LOAD_EMPTY_BUFFER;
		}
		return LOAD_ERROR;
	}

	eLoadState Context::load(const muon::String& filename)
	{
		std::ifstream file(filename.cStr());
		return load(file);
	}

	eCompilationState Context::compile()
	{
		eCompilationState compState = COMPILATION_SUCCESS;
		compState = parseLexical();
		if (compState == COMPILATION_SUCCESS)
		{
			compState = parseSyntaxic();
			if (compState == COMPILATION_SUCCESS)
			{
				compState = parseSemantic();
			}
		}

		m_loadBuffer.clear();
		return compState;
	}

	//==================================
	//			EXECUTION FLOW
	//==================================

	ePreparationState Context::prepare(const muon::String& func)
	{
		return PREPARATION_ERROR;
	}

	eExecutationState Context::execute()
	{
		/*
		auto it = m_byteCodeModules->find(module);
		if(it != m_byteCodeModules->end())
		{
			return run(it->second);
		}
		//*/
		return EXECUTION_ERROR;
	}

	//==================================
	//			EXECUTION
	//==================================

	eExecutationState Context::eval(const char* code, bool storeCode /* = false*/)
	{
		/*
		muon::system::Log log("VM", muon::LOG_DEBUG);
		muon::Variant nil;
		m_info.error.section = "#RunTimeEval#";

#if defined(MUON_DEBUG)
		muon::system::Time time;
		muon::f32 totalTime = 0;

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
		log() << "Lexical parse: " << time.now() << " seconds" << muon::endl;

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
		log() << "Syntaxic parse: " << time.now() << " seconds" << muon::endl;

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
		log() << "ByteCode Creation: " << time.now() << " seconds" << muon::endl;
		log() << "** Total Parse Time: " << totalTime << " seconds **" << muon::endl;
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
		return EXECUTION_ERROR;
	}

	eExecutationState Context::run(const ByteCode* buffer)
	{
		/*
		muon::system::Log log("VM");
#ifdef MUON_DEBUG
#	define DEBUG_CASE(Case) case Case:
		log(muon::LOG_DEBUG) << "** Execute Debug Log enabled **" << muon::endl;
#else
#	define DEBUG_CASE(Case) case Case:
#endif
		m_stack = 0;
		muon::u32 reg[3] = { 0, 0, 0 };

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
					muon::u64 type = m_instr.argG();
					muon::u8 dest = m_instr.res();
					muon::RawPointer rawData = muon::RawPointer(buffer + m_stack + 1);
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
					log(muon::LOG_ERROR) << "Nothing to do for OpCode: " << opCode << muon::endl;
				}
			}
			++m_stack;
		} while (exec);
		//*/
		return EXECUTION_ERROR;
	}
}

namespace
{
#define VM_REG(Id) (*registers)[Id]
	void arithmetic(hz::Context& vm, hz::eOpCode op, muon::u32 rA, muon::u32 rB, muon::u32 rC)
	{
		/*
		muon::f64 left = hz::boxed::getFloating(REG(rB));
		muon::f64 right = hz::boxed::getFloating(REG(rC));
		muon::f64 r = 0;
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
		muon::system::Log log("VM", muon::LOG_ERROR);
		log() << "Section: " << error.section << muon::endl;
		log() << "Function: " << error.function << " @ " << error.line << ":" << error.column << muon::endl;
		log() << "Message: \"" << error.message << "\"" << muon::endl;
	}
	//*/
}
