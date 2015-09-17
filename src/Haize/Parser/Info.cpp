
#include <Muon/Memory/Allocator.hpp>
#include <Muon/System/Log.hpp>

#include "Haize/Parser/Info.hpp"

namespace hz
{
	namespace parser
	{
		Info::Info()
			: impl(NULL)
			, TokenList(MUON_CNEW(std::vector<Token>))
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
			for(muon::u32 i = 0; i < TokenList->size(); ++i)
			{
				Token& t = (*TokenList)[i];
				if(t.value.getMeta() == MUON_META(muon::String))
				{
					MUON_CDELETE(t.value.get<muon::String*>());
				}
			}
			MUON_CDELETE(TokenList);
			MUON_CDELETE(ASTRoot);
		}
	}
}
