#include <fstream>
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
				error.clear();
				m_loadBuffer += buffer;
				error.state = Error::SUCCESS;
				return true;
			}

			bool DefaultCompiler::load(const m::String& file, Error& error)
			{
				error.clear();
				error.state = Error::ERROR;
				std::ifstream stream(file.cStr());
				if (!stream)
				{
					error.message = "Couldn't open the stream!";
					return false;
				}

				if (!stream.eof())
				{
					// Get stream size
					stream.seekg(0, stream.end);
					m::u64 length = (m::u64)stream.tellg();
					stream.seekg(0, stream.beg);

					char* buffer = (char*)malloc((m::u32)length + 1);
					stream.read(buffer, length);
					buffer[stream.gcount()] = 0;
					m_loadBuffer += buffer;
					free(buffer);

					error.state = Error::SUCCESS;
					return true;
				}
				error.message = "File stream seems to be empty: encountered EOF at beginning!";
				return false;
			}
		}
	}
}
