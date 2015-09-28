
#include <Muon/Memory/Allocator.hpp>
#include "Haize/Parser/Token.hpp"

namespace hz
{
	namespace parser
	{
		Token::Token()
			: type(S_INVALID)
			, category(S_INVALID)
			, section("#Undefined#")
			, function("#Undefined#")
			, line(0)
			, column(0)
		{
		}

		Token::Token(eTokenType type)
			: type(type)
			, category(S_INVALID)
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

		Token::~Token()
		{
			if(value.getMeta() == MUON_META(muon::String))
			{
				MUON_CDELETE(value.get<muon::String*>());
			}
		}

		Token& Token::operator=(const Token& token)
		{
			if (this != &token)
			{
				type = token.type;
				category = token.category;
				section = token.section;
				function = token.function;
				line = token.line;
				column = token.column;
				if (token.value.getMeta() == MUON_META(muon::String))
				{
					value = MUON_CNEW(muon::String, *token.value.get<muon::String*>());
				}
				else
				{
					value = token.value;
				}
			}
			return *this;
		}
	}
}
