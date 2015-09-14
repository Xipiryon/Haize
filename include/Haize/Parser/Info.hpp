#ifndef _HAIZE_PARSEINFO_INCLUDE
#define _HAIZE_PARSEINFO_INCLUDE

#include <deque>
#include "Haize/Core/Define.hpp"
#include "Haize/VM/ByteCode.hpp"
#include "Haize/Parser/Token.hpp"
#include "Haize/Parser/ASTNode.hpp"

namespace hz
{
	namespace parser
	{
		struct InfoImpl
		{
			// Empty struct used in each
			// step of parsing
		};

		/*!
		*
		*/
		struct InfoError
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
			InfoImpl* impl;

			InfoError error;
			std::vector<Token>* TokenList;
			ASTNode* ASTRoot;
			ByteCode* IRCode;
			muon::u32 IRCodeSize;
		};
	}
}

#endif
