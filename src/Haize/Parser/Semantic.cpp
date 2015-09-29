
#include <iostream>
#include <sstream>
#include <list>
#include <Muon/Memory/Allocator.hpp>
#include <Muon/System/Log.hpp>
#include "Haize/Parser/Semantic.hpp"
#include "Haize/VM/SymbolTable.hpp"

namespace
{
	bool generateByteCode(hz::parser::Info&);
	void pushRawByteCode(hz::parser::Info& info, muon::u8 dest, muon::u64 metaId, const muon::RawPointer& data);
}

namespace hz
{
	namespace parser
	{
		/*
		* Custom ParseInfoImpl
		* Specific for Syntaxic parsing
		*/
		struct InfoImplSemantic : InfoImpl
		{
			std::list<muon::u32>* childIndex;
			std::list<ByteCode>* bytecode;
			SymbolTable* symbols;
		};
#define INFO_IMPL ((hz::parser::InfoImplSemantic*)info.impl)

		namespace semantic
		{

			bool parse(Info& info)
			{
				if (info.IRCode)
				{
					::free(info.IRCode);
					info.IRCodeSize = 0;
				}
				info.impl = MUON_CNEW(InfoImplSemantic);
				INFO_IMPL->childIndex = MUON_CNEW(std::list<muon::u32>);
				INFO_IMPL->bytecode = MUON_CNEW(std::list<ByteCode>);
				INFO_IMPL->symbols = MUON_CNEW(SymbolTable);

				INFO_IMPL->childIndex->push_back(0);

				if (generateByteCode(info))
				{
					muon::u32 offset = 0;
					info.IRCodeSize = sizeof(ByteCode) * (INFO_IMPL->bytecode->size() + 1);
					info.IRCode = (ByteCode*)::malloc(info.IRCodeSize);
					for (auto it = INFO_IMPL->bytecode->begin(); it != INFO_IMPL->bytecode->end(); ++it)
					{
						*(info.IRCode + offset) = *it;
						++offset;
					}
					(info.IRCode + offset)->opCode(hz::SYS_PRG_END);
				}

				MUON_CDELETE(INFO_IMPL->childIndex);
				MUON_CDELETE(INFO_IMPL->bytecode);
				MUON_CDELETE(INFO_IMPL->symbols);
				MUON_CDELETE(INFO_IMPL);
				return info.error.message.empty();
			}

			void display(Info& info)
			{
#ifdef MUON_DEBUG
#endif
			}
		}
	}
}

namespace
{
	template<typename ...Args>
	void pushByteCode(hz::parser::Info& info, Args...args)
	{
		hz::ByteCode bc;
		bc.setup(args...);
		INFO_IMPL->bytecode->push_back(bc);
	}


	void pushRawByteCode(hz::parser::Info& info, muon::u8 dest, muon::u64 metaId, const muon::RawPointer& data)
	{
		/*
		hz::ByteCode bc;
		bc.setup(hz::eOpCode::SYS_SETRAW, dest, data);
		INFO_IMPL->bytecode->push_back(bc);
		//Must split in two bytecode of 4 byte
		muon::u64 raw = box.getInteger();
		bc.data = raw;
		INFO_IMPL->bytecode->push_back(bc);
		//*/
		/*
		muon::u32 lside = (raw & 0xFFffFFff00000000) >> 32;
		muon::u32 rside = (raw & 0x00000000FFffFFff);
		bc.data = lside;
		INFO_IMPL->bytecode->push_back(bc);
		bc.data = rside;
		INFO_IMPL->bytecode->push_back(bc);
		//*/
	}

	bool generateByteCode(hz::parser::Info& info)
	{
		bool success = true;
		hz::parser::ASTNode* node = info.ASTRoot;

		muon::system::Log log("Semantic", muon::LOG_ERROR);
		/*
		while (node = down(info, node))
		{
			switch (node->token.type)
			{
				case hz::SYS_PRG_END:
					break;
				default:
					log() << "Unhandled code " << node->name << muon::endl;
					break;
			}

			if (!node)
			{
				break;
			}

			if (!success)
			{
				return false;
			}
		}
		// */
		return success;
	}
}
