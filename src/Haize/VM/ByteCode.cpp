
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
	const muon::u32 OFFSET_USE_ARGG = 30;
	const muon::u32 OFFSET_OPCODE = 24;
	const muon::u32 OFFSET_RES = 16;
	const muon::u32 OFFSET_ARG1 = 8;
	const muon::u32 OFFSET_ARG2 = 0;
	const muon::u32 OFFSET_ARGG = 0;

	const muon::u32 MASK_USE_ARGG = (0x40 << OFFSET_OPCODE);
	const muon::u32 MASK_OPCODE = (0x3F << OFFSET_OPCODE);
	const muon::u32 MASK_RES = (0xFF << OFFSET_RES);
	const muon::u32 MASK_ARG1 = (0xFF << OFFSET_ARG1);
	const muon::u32 MASK_ARG2 = (0xFF << OFFSET_ARG2);
	const muon::u32 MASK_ARGG = (MASK_ARG1 | MASK_ARG2);
}

namespace hz
{
	eOpCode ByteCode::opCode() const
	{
		return (eOpCode)((data & MASK_OPCODE) >> OFFSET_OPCODE);
	}

	muon::u8 ByteCode::res() const
	{
		return ((data & MASK_RES) >> OFFSET_RES);
	}

	muon::u8 ByteCode::arg1() const
	{
		return ((data & MASK_ARG1) >> OFFSET_ARG1);
	}

	muon::u8 ByteCode::arg2() const
	{
		return ((data & MASK_ARG2) >> OFFSET_ARG2);
	}

	muon::u16 ByteCode::argG() const
	{
		return ((data & MASK_ARGG) >> OFFSET_ARGG);
	}

	void ByteCode::opCode(eOpCode op)
	{
		muon::u8 iop = 0 | op;
		data = (data & ~MASK_OPCODE) | (iop << OFFSET_OPCODE);
	}

	void ByteCode::res(muon::u8 reg)
	{
		data = (data & ~MASK_RES) | (reg << OFFSET_RES);
	}

	void ByteCode::arg1(muon::u8 reg)
	{
		data = (data & ~MASK_ARG1) | (reg << OFFSET_ARG1);
	}

	void ByteCode::arg2(muon::u8 reg)
	{
		data = (data & ~MASK_ARG2) | (reg << OFFSET_ARG2);
	}

	void ByteCode::argG(muon::u16 reg)
	{
		data = (data & ~MASK_ARGG) | (reg << OFFSET_ARGG);
	}

	void ByteCode::setup(eOpCode opcode, muon::u8 A, muon::u8 B, muon::u8 C)
	{
		opCode(opcode);
		res(A);
		arg1(B);
		arg2(C);
	}

	void ByteCode::setup(eOpCode opcode, muon::u8 A, muon::u16 G)
	{
		opCode(opcode);
		res(A);
		argG(G);
	}

	void ByteCode::setup(eOpCode opcode, muon::u8 A)
	{
		opCode(opcode);
		res(A);
	}
}

