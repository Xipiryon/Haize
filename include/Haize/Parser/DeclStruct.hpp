#ifndef INCLUDE_HAIZE_DECLSTRUCT_INCLUDED
#define INCLUDE_HAIZE_DECLSTRUCT_INCLUDED

#include <unordered_map>
#include <Muon/String.hpp>
#include "Haize/Core/Define.hpp"

namespace hz
{
	namespace parser
	{
		struct DeclVariable
		{
			m::String declType;
			m::String declName;
		};

		struct DeclFunction
		{
			m::String declType;
			m::String declName;
			std::vector<DeclVariable> declParams;
		};

		struct DeclClass
		{
			m::String declName;
			std::unordered_map<m::String, DeclFunction> declMethods;
			std::unordered_map<m::String, DeclVariable> declMembers;
		};

		struct DeclNamespace
		{
			m::String declName;
			std::unordered_map<m::String, DeclNamespace*> declNamespaces;
			std::unordered_map<m::String, DeclVariable> declGlobals;
			std::unordered_map<m::String, DeclFunction> declFunctions;
			std::unordered_map<m::String, DeclClass> declClasses;
		};
	}
}

#endif
