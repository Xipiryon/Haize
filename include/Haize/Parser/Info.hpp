#ifndef INCLUDE_HAIZE_INFO_INCLUDED
#define INCLUDE_HAIZE_INFO_INCLUDED

#include <deque>
#include "Haize/Core/Define.hpp"
#include "Haize/VM/ByteCode.hpp"
#include "Haize/Parser/Token.hpp"
#include "Haize/Parser/ASTNode.hpp"

namespace hz
{
	namespace parser
	{
		/*!
		*
		*/
		struct HAIZE_API InfoError
		{
			muon::String message;
			muon::String section;
			muon::String function;
			muon::u32 line;
			muon::u32 column;
		};
	}
}

#endif
