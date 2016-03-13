#ifndef INCLUDE_HAIZE_ICOMPILER_INCLUDED
#define INCLUDE_HAIZE_ICOMPILER_INCLUDED

#include "Haize/Error.hpp"

namespace hz
{
	namespace parser
	{
		class HAIZE_API ICompiler
		{
		public:
			virtual bool load(const char* buffer, Error& error) = 0;
			virtual bool load(const m::String& file, Error& error) = 0;

			virtual bool compile(Error& error) = 0;
		protected:
			virtual bool lexical(Error& error) = 0;
			virtual bool syntaxic(Error& error) = 0;
			virtual bool semantic(Error& error) = 0;
		};
	}
}
#endif
