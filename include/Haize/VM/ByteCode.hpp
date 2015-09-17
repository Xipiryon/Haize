#ifndef _HAIZE_BYTECODE_INCLUDE
#define _HAIZE_BYTECODE_INCLUDE

#include <Muon/Core/Typedef.hpp>
#include "Haize/VM/OpCode.hpp"

namespace hz
{
	struct HAIZE_API ByteCode
	{
		static const muon::u8 INVALID_REG = -1;
		static const muon::u8 RETURN_REG = 0;
		static const muon::u8 USABLE_REG = 1;

		eOpCode opCode() const;
		muon::u8 res() const;
		muon::u8 arg1() const;
		muon::u8 arg2() const;
		muon::u16 argG() const;

		void opCode(eOpCode);
		void res(muon::u8);
		void arg1(muon::u8);
		void arg2(muon::u8);
		void argG(muon::u16);

		void setup(eOpCode, muon::u8 res, muon::u8 arg1, muon::u8 arg2);
		void setup(eOpCode, muon::u8 res, muon::u16 argG);
		void setup(eOpCode, muon::u8 res);

		muon::u32 data = 0;
	};
}
#endif
