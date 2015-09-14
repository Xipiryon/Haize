
#include <Muon/Memory/Allocator.hpp>

#include "Haize/Parser/Token.hpp"

namespace hz
{
	namespace parser
	{
		Token::Token(eTokenType type)
			: type(type)
			, section("#Undefined#")
			, function("#Undefined#")
			, line(0)
			, column(0)
		{
		}

		Token::Token(const Token& token)
		{
			*this = token;
		}

		Token& Token::operator=(const Token& token)
		{
			if (this != &token)
			{
				type = token.type;
				section = token.section;
				function = token.function;
				line = token.line;
				column = token.line;
				value = token.value;
			}
			return *this;
		}
	}
}
