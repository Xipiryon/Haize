#ifndef INCLUDE_HAIZE_BYTECODE_INCLUDE
#define INCLUDE_HAIZE_BYTECODE_INCLUDE

#include <Muon/Core/Typedef.hpp>
#include "Haize/VM/OpCode.hpp"

namespace hz
{
	struct HAIZE_API ByteCode
	{
		static const muon::u8 REG_INVALID = 		0x00;			//!< Invalid register index
		static const muon::u8 REG_RETURN = 			0x01;			//!< Special register used for return value
		static const muon::u8 REG_USABLE = 			0xFE;			//!< Every other usable register
		static const muon::u8 REG_MAX_AVAILABLE = 	REG_USABLE-1;	//!< Max register index value available

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
