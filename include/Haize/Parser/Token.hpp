#ifndef INCLUDE_HAIZE_TOKEN_INCLUDED
#define INCLUDE_HAIZE_TOKEN_INCLUDED

#include <Muon/Variant.hpp>
#include "Haize/Core/Define.hpp"

namespace hz
{
	namespace parser
	{
		/*!
		*
		*/
		struct HAIZE_API Token
		{
			Token()
				: type(0)
				, line(0)
				, column(0)
			{
			}

			Token(m::u32 type_, m::u32 line_, m::u32 column_)
				: type(type_)
				, line(line_)
				, column(column_)
			{
			}

			Token(const Token& t)
			{
				*this = t;
			}

			Token& operator=(const Token& t)
			{
				if (&t != this)
				{
					type = t.type;
					value = t.value;
					line = t.line;
					column = t.column;
				};
				return *this;
			}

			m::u32 type;
			m::Variant value;

			m::u32 line;
			m::u32 column;
		};
	}
}

#endif
