
#include <cstdlib>
#include <sstream>
#include <vector>
#include <Muon/System/Log.hpp>
#include <Muon/System/Time.hpp>
#include "Haize/VM/OpCode.hpp"
#include "Haize/Parser/Semantic.hpp"
#include "Haize/Parser/Info.hpp"

//Require the include to the parser folder, relative to this one
#include "./Parser/YACC/generated/yacc.haize.hpp"
#include "./Parser/YACC/Extern.hpp"

#include "Haize/VM.hpp"

namespace
{
	using namespace muon;
	void arithmetic(hz::VMInstance&, hz::eOpCode, muon::u32, muon::u32, muon::u32);
	void printError(hz::parser::InfoError& error);
}

namespace hz
{
	VMInstance::VMInstance()
		: _stack(0)
	{
	}

	VMInstance::~VMInstance()
	{
	}

	muon::Variant VMInstance::eval(const char* code)
	{
		muon::system::Log log("VM", muon::LOG_DEBUG);
		muon::Variant nil;
		_info.error.section = "#RunTimeEval#";

		muon::system::Time time;
		muon::f32 totalTime = 0;

		// Lexical & Syntaxic
		time.start();
		{
			_info.ASTRoot->name = "NT_ROOT";
			_info.ASTRoot->type = NT_ROOT;
			g_parseInfo = &_info;
			custom_yymain(code);
		}
		totalTime += time.now();
		log() << "AST Creation: " << time.now() << " seconds" << muon::endl;

		// Semantic
		time.start();
		{
			if (!hz::parser::semantic::parse(_info))
			{
				printError(_info.error);
				return nil;
			}
			hz::parser::semantic::display(_info);
		}
		totalTime += time.now();
		log() << "ByteCode Creation: " << time.now() << " seconds" << muon::endl;
		log() << "** Total Parse Time: " << totalTime << " seconds **" << muon::endl;

		// Execution
		return execute(_info.IRCode);
	}

	bool VMInstance::load(const std::istream& filename)
	{
		return false;
	}

	bool VMInstance::load(const char* filename)
	{
		return false;
	}

	bool VMInstance::compile(const muon::String& module)
	{
		return false;
	}

	muon::Variant VMInstance::execute(const ByteCode* buffer)
	{
		muon::system::Log log("VM");
#ifdef MUON_DEBUG
#	define DEBUG_CASE(Case) case Case:
		log(muon::LOG_DEBUG) << "** Execute Debug Log enabled **" << muon::endl;
#else
#	define DEBUG_CASE(Case) case Case:
#endif
		_stack = 0;
		muon::u32 reg[3] = { 0, 0, 0 };

		bool exec = true;
		do
		{
			_instr = *(buffer + _stack);
			eOpCode opCode = _instr.opCode();
			switch (opCode)
			{
				DEBUG_CASE(SYS_MOV)
				{
					//(*registers)[instr.res()] = (*registers)[instr.argG()];
					break;
				}

				DEBUG_CASE(SYS_SETRAW)
				{
					muon::u64 type = _instr.argG();
					muon::u8 dest = _instr.res();
					muon::RawPointer rawData = muon::RawPointer(buffer + _stack + 1);
					//_registers[dest].set(rawData);
					_stack += 2;
					break;
				}

				// *****
				DEBUG_CASE(MATH_ADD)
				DEBUG_CASE(MATH_SUB)
				DEBUG_CASE(MATH_MUL)
				DEBUG_CASE(MATH_DIV)
				{
					arithmetic(*this, opCode, _instr.res(), _instr.arg1(), _instr.arg2());
					break;
				}
				DEBUG_CASE(MATH_MOD)
				{
					break;
				}
				DEBUG_CASE(MATH_ASN)
				{
					_registers[_instr.res()] = _registers[_instr.argG()];
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
					_registers[_instr.res()] = _registers[_instr.argG()];
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
			++_stack;
		} while (exec);

		return _registers[ByteCode::RETURN_REG];
	}

	muon::Variant VMInstance::execute()
	{
		return muon::Variant();
	}
}

namespace
{
#define VM_REG(Id) (*registers)[Id]
	void arithmetic(hz::VMInstance& vm, hz::eOpCode op, muon::u32 rA, muon::u32 rB, muon::u32 rC)
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

	void printError(hz::parser::InfoError& error)
	{
		muon::system::Log log("VM", muon::LOG_ERROR);
		log() << "Section: " << error.section << muon::endl;
		log() << "Function: " << error.function << " @ " << error.line << ":" << error.column << muon::endl;
		log() << "Message: \"" << error.message << "\"" << muon::endl;
	}
}
