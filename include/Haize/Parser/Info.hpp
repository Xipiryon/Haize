#ifndef _HAIZE_PARSEINFO_INCLUDE
#define _HAIZE_PARSEINFO_INCLUDE

#include <deque>
#include "Haize/Core/Define.hpp"
#include "Haize/VM/ByteCode.hpp"
#include "Haize/Parser/ASTNode.hpp"

namespace hz
{
	namespace parser
	{
		/*!
		*
		*/
		struct ParseInfoImpl;

		struct ParseInfoError
		{
			String message;
			String section;
			String function;
			u32 line;
			u32 column;
		};

		/*!
		*
		*/
		struct HAIZE_API Info
		{
			Info();
			~Info();
			ParseInfoImpl* impl;

			ParseInfoError error;
			ASTNode* ASTRoot;
			ByteCode* IRCode;
			muon::u32 IRCodeSize;
		};
	}
}

#endif
