#include <sstream>
#include <Muon/System/Log.hpp>

#include "Haize/Parser/Priv/DefaultCompiler.hpp"

namespace hz
{
	namespace parser
	{
		namespace priv
		{
			bool DefaultCompiler::load(const char* buffer, Error& error)
			{
				return true;
			}

			bool DefaultCompiler::load(const m::String& file, Error& error)
			{
				return true;
			}
		}
	}
}
