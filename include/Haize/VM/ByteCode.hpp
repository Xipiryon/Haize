#ifndef INCLUDE_HAIZE_BYTECODE_INCLUDE
#define INCLUDE_HAIZE_BYTECODE_INCLUDE

#include <Muon/Core/Typedef.hpp>
#include "Haize/VM/OpCode.hpp"

namespace hz
{
	struct HAIZE_API ByteCode
	{
		static const m::u8 REG_INVALID = 		0x00;			//!< Invalid register index
		static const m::u8 REG_RETURN = 			0x01;			//!< Special register used for return value
		static const m::u8 REG_USABLE = 			0xFE;			//!< Every other usable register
		static const m::u8 REG_MAX_AVAILABLE = 	REG_USABLE-1;	//!< Max register index value available

		eOpCode opCode() const;
		m::u8 res() const;
		m::u8 arg1() const;
		m::u8 arg2() const;
		m::u16 argG() const;

		void opCode(eOpCode);
		void res(m::u8);
		void arg1(m::u8);
		void arg2(m::u8);
		void argG(m::u16);

		void setup(eOpCode, m::u8 res, m::u8 arg1, m::u8 arg2);
		void setup(eOpCode, m::u8 res, m::u16 argG);
		void setup(eOpCode, m::u8 res);

		m::u32 data = 0;
	};
}
#endif
