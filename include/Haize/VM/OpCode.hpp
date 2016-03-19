
#ifndef INCLUDE_HAIZE_OPCODE_INCLUDE
#define INCLUDE_HAIZE_OPCODE_INCLUDE

#include "Haize/Core/Config.hpp"

namespace hz
{
	enum eOpCode
	{
		SYS_MOV = 0,	//! Copy the content of a register into another
		SYS_SETRAW,		//! Set in a register a boxed value, stored in the next 64 bits
		// *****
		MATH_ADD,		//! +
		MATH_SUB,		//! -
		MATH_MUL,		//! *
		MATH_DIV,		//! /
		MATH_MOD,		//! %
		MATH_ASN,		//! =
		// *****
		BIT_LSH,		//! <<
		BIT_RSH,		//! >>
		// *****
		BIT_AND,		//! &
		BIT_OR,			//! |
		BIT_XOR,		//! ^
		BIT_NOT,		//! ~
		// *****
		CMP_NOT,		//! !
		CMP_LT,			//! <
		CMP_LET,		//! <=
		CMP_EQU,		//! ==
		CMP_AND,		//! &&
		CMP_OR,			//! ||
		// *****
		SYS_JUMP,		//! Jump to a given address
		SYS_RETURN,		//! Return from a function
		SYS_CALL,		//! Function call
		// *****
		SYS_PRG_END,	//! Indicate that program must end
		// *****
		TOTAL_COUNT		//! Total counter of OpCode (Internal Value)
	};

	static const char* OpCodeStr[eOpCode::TOTAL_COUNT] =
	{
		"SYS_MOV",
		"SYS_SETRAW",
		// *****
		"MATH_ADD",
		"MATH_SUB",
		"MATH_MUL",
		"MATH_DIV",
		"MATH_MOD",
		"MATH_ASN",
		// *****
		"BIT_SHL",
		"BIT_SHR",
		// *****
		"BIT_AND",
		"BIT_OR",
		"BIT_XOR",
		"BIT_NOT",
		// *****
		"CMP_NOT",
		"CMP_LT",
		"CMP_LET",
		"CMP_EQU",
		"CMP_AND",
		"CMP_OR",
		// *****
		"SYS_JUMP",
		"SYS_RETURN",
		"SYS_CALL",
		// *****
		"SYS_PRG_END",
		// *****
	};
}
#endif
