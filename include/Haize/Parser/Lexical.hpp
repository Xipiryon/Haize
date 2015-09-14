#ifndef _HAIZE_LEXICAL_INCLUDE
#define _HAIZE_LEXICAL_INCLUDE

#include "Haize/Parser/Info.hpp"

namespace hz
{
	namespace parser
	{
		/*!
		* Lexical Parser namespace
		* Contains everything require to convert an input stream
		* to a TokenList.
		*/
		namespace lexical
		{
			/*!
			*
			*/
			HAIZE_API bool parse(Info&, const char*);

			/*!
			*
			*/
			HAIZE_API bool parse(Info&, std::istream&);

			/*!
			*
			*/
			HAIZE_API void display(Info&);
		}
	}
}

#endif
