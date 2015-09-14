#ifndef _HAIZE_SYNTAXIC_INCLUDE
#define _HAIZE_SYNTAXIC_INCLUDE

#include "Haize/Parser/Info.hpp"

namespace hz
{
	namespace parser
	{
		/*!
		* Syntaxic Parser namespace
		* Given a Token list as input, it will generate the
		* parse tree required for the semantic parsing.
		*/
		namespace syntaxic
		{
			/*!
			*
			*/
			HAIZE_API bool parse(parser::Info&);

			/*!
			*
			*/
			HAIZE_API void display(parser::Info&);
		}
	}
}

#endif
