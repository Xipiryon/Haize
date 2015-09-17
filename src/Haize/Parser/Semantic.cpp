
#include <iostream>
#include <sstream>
#include <list>
#include <Muon/Memory/Allocator.hpp>
#include <Muon/System/Log.hpp>
#include "Haize/Parser/Semantic.hpp"
#include "Haize/VM/SymbolTable.hpp"

namespace
{
	using namespace muon;
	hz::parser::ASTNode* up(hz::parser::Info&, hz::parser::ASTNode*);
	hz::parser::ASTNode* down(hz::parser::Info&, hz::parser::ASTNode*);
	hz::parser::ASTNode* next(hz::parser::Info&, hz::parser::ASTNode*);

	bool generateByteCode(hz::parser::Info&);
	hz::parser::ASTNode* process(hz::parser::Info&, hz::parser::ASTNode*, muon::u8(*)(hz::parser::Info&, hz::parser::ASTNode*), bool&);

	muon::u8 generateASN(hz::parser::Info&, hz::parser::ASTNode*);
	muon::u8 generateLVal(hz::parser::Info&, hz::parser::ASTNode*);
	muon::u8 generateRVal(hz::parser::Info&, hz::parser::ASTNode*);
	muon::u8 generateExpr(hz::parser::Info&, hz::parser::ASTNode*);
	muon::u8 generateSubexpr(hz::parser::Info&, hz::parser::ASTNode*);
	muon::u8 generateRetstat(hz::parser::Info&, hz::parser::ASTNode*);

	void pushRawByteCode(hz::parser::Info& info, muon::u8 dest, muon::u64 metaId, const RawPointer& data);
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


	void pushRawByteCode(hz::parser::Info& info, muon::u8 dest, muon::u64 metaId, const RawPointer& data)
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

	hz::parser::ASTNode* up(hz::parser::Info& info, hz::parser::ASTNode* node)
	{
		INFO_IMPL->childIndex->pop_back();
		return node->parent;
	}

	hz::parser::ASTNode* down(hz::parser::Info& info, hz::parser::ASTNode* node)
	{
		hz::parser::ASTNode* downNode = NULL;
		muon::u32 id = INFO_IMPL->childIndex->back();
		if (id < node->children->size())
		{
			downNode = node->children->at(id);
			INFO_IMPL->childIndex->push_back(0);
			return downNode;
		}
		return downNode;
	}

	hz::parser::ASTNode* next(hz::parser::Info& info, hz::parser::ASTNode* node)
	{
		muon::u32 id = ++INFO_IMPL->childIndex->back();
		if (id < node->children->size())
		{
			return node->children->at(id);
		}
		return NULL;
	}

	hz::parser::ASTNode* process(hz::parser::Info& info, hz::parser::ASTNode* node, muon::u8(*f)(hz::parser::Info&, hz::parser::ASTNode*), bool& success)
	{
		success = (f(info, node) != hz::ByteCode::INVALID_REG);
		hz::parser::ASTNode* nextNode = next(info, node);
		//While we didn't find a next node
		while (!nextNode)
		{
			//Try to search from upper
			nextNode = up(info, nextNode);
			//No parent? Then no more node to parse, return
			if (!nextNode)
			{
				return NULL;
			}
		}

		return nextNode;
	}

	bool generateByteCode(hz::parser::Info& info)
	{
		bool success = true;
		hz::parser::ASTNode* node = info.ASTRoot;

		muon::system::Log log("Semantic", muon::LOG_ERROR);
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
		return success;
	}

	muon::u8 generateASN(hz::parser::Info& info, hz::parser::ASTNode* node)
	{
		hz::parser::ASTNode* asn = node->children->at(0);
		muon::u8 dest = generateLVal(info, node->children->at(1));
		muon::u16 src = generateExpr(info, node->children->at(2));

		pushByteCode(info, hz::MATH_ASN, dest, src);
		return dest;
	}

	muon::u8 generateLVal(hz::parser::Info& info, hz::parser::ASTNode* node)
	{
		hz::parser::ASTNode* child = node->children->front();
		//muon::u8 dest = INFO_IMPL->symbols->checkOrCreateRegister(child->token.value.cStr());
		return hz::ByteCode::INVALID_REG;
	}

	muon::u8 generateRVal(hz::parser::Info& info, hz::parser::ASTNode* node)
	{
		muon::u8 r = hz::ByteCode::INVALID_REG;
		/*
		hz::parser::TokenNode* child = node->child->front();
		if (child->token.type == hz::NT_LVAL)
		{
			r = generateLVal(info, child);
		}
		else
		{
			r = INFO_IMPL->symbols->checkOrCreateRegister(child->token.value.cStr());
			hz::BoxedValue box = child->token.value;
			pushRawByteCode(info, r, box);
		}
		//*/
		return r;
	}

	muon::u8 generateExpr(hz::parser::Info& info, hz::parser::ASTNode* node)
	{
		muon::u8 r = hz::ByteCode::INVALID_REG;
		/*
		hz::parser::TokenNode* child = node->child->front();
		switch (child->token.type)
		{
			case hz::NT_SUBEXPR:
				r = generateSubexpr(info, child);
				break;
		}
		//*/
		return r;
	}

	muon::u8 generateSubexpr(hz::parser::Info& info, hz::parser::ASTNode* node)
	{
		muon::u8 r = hz::ByteCode::INVALID_REG;
		/*
		hz::parser::TokenNode* child = node->child->front();
		switch (child->token.type)
		{
			case hz::NT_RVAL:
				r = generateRVal(info, child);
				break;
		}
		//*/
		return r;
	}

	muon::u8 generateRetstat(hz::parser::Info& info, hz::parser::ASTNode* node)
	{
		muon::u8 dest = generateRVal(info, node->children->at(1));
		pushByteCode(info, hz::SYS_RETURN, 0, dest);
		return dest;
	}
}
