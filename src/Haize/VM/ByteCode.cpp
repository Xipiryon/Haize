
#include "Haize/VM/ByteCode.hpp"

/*
* Note that:
* First bit is currently unused
* [1: ?][1: useG?][6: opcode][8: res][8: arg1][8: arg2]
* [1: ?][1: useG?][6: opcode][8: res][16: argG]
*/

namespace
{
	using namespace muon;
	const u32 OFFSET_USE_ARGG = 30;
	const u32 OFFSET_OPCODE = 24;
	const u32 OFFSET_RES = 16;
	const u32 OFFSET_ARG1 = 8;
	const u32 OFFSET_ARG2 = 0;
	const u32 OFFSET_ARGG = 0;

	const u32 MASK_USE_ARGG = (0x40 << OFFSET_OPCODE);
	const u32 MASK_OPCODE = (0x3F << OFFSET_OPCODE);
	const u32 MASK_RES = (0xFF << OFFSET_RES);
	const u32 MASK_ARG1 = (0xFF << OFFSET_ARG1);
	const u32 MASK_ARG2 = (0xFF << OFFSET_ARG2);
	const u32 MASK_ARGG = (MASK_ARG1 | MASK_ARG2);
}

namespace hz
{
	eOpCode ByteCode::opCode() const
	{
		return (eOpCode)((data & MASK_OPCODE) >> OFFSET_OPCODE);
	}

	u8 ByteCode::res() const
	{
		return ((data & MASK_RES) >> OFFSET_RES);
	}

	u8 ByteCode::arg1() const
	{
		return ((data & MASK_ARG1) >> OFFSET_ARG1);
	}

	u8 ByteCode::arg2() const
	{
		return ((data & MASK_ARG2) >> OFFSET_ARG2);
	}

	u16 ByteCode::argG() const
	{
		return ((data & MASK_ARGG) >> OFFSET_ARGG);
	}

	void ByteCode::opCode(eOpCode op)
	{
		u8 iop = 0 | op;
		data = (data & ~MASK_OPCODE) | (iop << OFFSET_OPCODE);
	}

	void ByteCode::res(u8 reg)
	{
		data = (data & ~MASK_RES) | (reg << OFFSET_RES);
	}

	void ByteCode::arg1(u8 reg)
	{
		data = (data & ~MASK_ARG1) | (reg << OFFSET_ARG1);
	}

	void ByteCode::arg2(u8 reg)
	{
		data = (data & ~MASK_ARG2) | (reg << OFFSET_ARG2);
	}

	void ByteCode::argG(u16 reg)
	{
		data = (data & ~MASK_ARGG) | (reg << OFFSET_ARGG);
	}

	void ByteCode::setup(eOpCode opcode, u8 A, u8 B, u8 C)
	{
		opCode(opcode);
		res(A);
		arg1(B);
		arg2(C);
	}

	void ByteCode::setup(eOpCode opcode, u8 A, u16 G)
	{
		opCode(opcode);
		res(A);
		argG(G);
	}

	void ByteCode::setup(eOpCode opcode, u8 A)
	{
		opCode(opcode);
		res(A);
	}
}

