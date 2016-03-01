
#include <Muon/Memory/Allocator.hpp>
#include "Haize/Parser/Token.hpp"

namespace hz
{
	namespace parser
	{
		Token::Token()
			: type(S_INVALID)
			, category(S_INVALID)
			, line(0)
			, column(0)
		{
		}

		Token::Token(eTokenType type)
			: type(type)
			, category(S_INVALID)
			, line(0)
			, column(0)
		{
		}

		Token::Token(const Token& token)
		{
			*this = token;
		}

		Token::~Token()
		{
		}

		Token& Token::operator=(const Token& token)
		{
			if (this != &token)
			{
				type = token.type;
				category = token.category;
				line = token.line;
				column = token.column;
				value = token.value;
			}
			return *this;
		}
	}
}
