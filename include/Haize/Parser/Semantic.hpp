#ifndef _HAIZE_SEMANTIC_INCLUDE
#define _HAIZE_SEMANTIC_INCLUDE

#include "Haize/Parser/Info.hpp"

namespace hz
{
	namespace parser
	{
		/*!
		* Semantic Parser namespace
		* Contains everything require to convert an abstract syntax tree
		* to the final ByteCode buffer used by the VMInstance
		*/
		namespace sem
		{
			/*!
			*
			*/
			HAIZE_API bool parse(Info&);

			/*!
			*
			*/
			HAIZE_API void display(Info&);
		}
	}
}

#endif
