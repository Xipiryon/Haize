#ifndef _HAIZE_BYTECODE_INCLUDE
#define _HAIZE_BYTECODE_INCLUDE

#include <Muon/Core/Typedef.hpp>
#include "Haize/VM/OpCode.hpp"

namespace hz
{
	struct HAIZE_API ByteCode
	{
		static const u8 INVALID_REG = -1;
		static const u8 RETURN_REG = 0;
		static const u8 USABLE_REG = 1;

		eOpCode opCode() const;
		u8 res() const;
		u8 arg1() const;
		u8 arg2() const;
		u16 argG() const;

		void opCode(eOpCode);
		void res(u8);
		void arg1(u8);
		void arg2(u8);
		void argG(u16);

		void setup(eOpCode, u8 res, u8 arg1, u8 arg2);
		void setup(eOpCode, u8 res, u16 argG);
		void setup(eOpCode, u8 res);

		u32 data = 0;
	};
}
#endif
