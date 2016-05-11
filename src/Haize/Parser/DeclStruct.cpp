#include <Muon/Memory/Allocator.hpp>
#include <Muon/System/Log.hpp>
#include "Haize/Parser/DeclStruct.hpp"
#include "Haize/Parser/ASTNode.hpp"
#include "Haize/Interpreter/ByteCode.hpp"

namespace hz
{
	namespace parser
	{
		const m::String DeclNamespace::g_GlobalNamespaceName = "#ROOT#";
	}
}
