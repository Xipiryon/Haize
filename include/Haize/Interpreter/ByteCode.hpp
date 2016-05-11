#ifndef INCLUDE_HAIZE_BYTECODE_INCLUDE
#define INCLUDE_HAIZE_BYTECODE_INCLUDE

#include <Muon/Core/Typedef.hpp>
#include "Haize/Interpreter/OpCode.hpp"

namespace hz
{
	struct HAIZE_API ByteCode
	{
		static const m::u8 REG_INVALID = 0x00;					//!< Invalid register index
		static const m::u8 REG_RETURN = 0x01;					//!< Special register used for return value
		static const m::u8 REG_USABLE = 0xFE;					//!< Every other usable register
		static const m::u8 REG_MAX_AVAILABLE = REG_USABLE - 1;	//!< Max register index value available

		eOpCode getOpCode() const;
		m::u8 getRegResult() const;
		m::u8 getRegArg1() const;
		m::u8 getRegArg2() const;
		m::u16 getRegArgG() const;

		void setOpCode(eOpCode);
		void setRegResult(m::u8);
		void setRegArg1(m::u8);
		void setRegArg2(m::u8);
		void setRegArgG(m::u16);

		void setup(eOpCode op, m::u8 regResult, m::u8 regArg1, m::u8 regArg2);
		void setup(eOpCode op, m::u8 regResult, m::u16 regArgG);

	private:
		m::u32 data = 0;
	};
}
#endif
