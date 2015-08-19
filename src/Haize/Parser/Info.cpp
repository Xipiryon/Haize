
#include <Muon/Memory/Allocator.hpp>
#include <Muon/System/Log.hpp>

#include "Haize/Parser/Info.hpp"

namespace hz
{
	namespace parser
	{
		Info::Info()
			: impl(NULL)
			, ASTRoot(MUON_CNEW(ASTNode))
			, IRCode(NULL)
			, IRCodeSize(0)
		{
			error.section = "#Undefined#";
			error.function = "#Undefined#";
			error.line = 0;
			error.column = 0;
			error.message.clear();
		}

		Info::~Info()
		{
			MUON_CDELETE(ASTRoot);
		}
	}
}
