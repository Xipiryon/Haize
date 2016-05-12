#ifndef INCLUDE_HAIZE_DECLSTRUCT_INCLUDED
#define INCLUDE_HAIZE_DECLSTRUCT_INCLUDED

#include <map>
#include <Muon/String.hpp>
#include <Muon/Variant.hpp>
#include "Haize/Runtime/ByteCode.hpp"

namespace hz
{
	namespace parser
	{
		struct ASTNodeFunctionDecl;
		struct ASTNodeFunctionCall;

		struct DeclVariable
		{
			union
			{
				bool global;
				m::u32 prefix;
			};
			m::String type;
			m::String name;
			m::Variant value;
		};

		struct DeclFunction
		{
			union
			{
				m::u32 addrOffset;
				ByteCode* addrPtr;
			};
			m::String type;
			m::String name;
			std::vector<DeclVariable> params;
		};

		struct DeclClass
		{
			m::String name;
			std::map<m::String, DeclFunction> methods;
			std::map<m::String, DeclVariable> members;
		};

		struct DeclNamespace
		{
			static const m::String g_GlobalNamespaceName;
			m::String name;
			DeclNamespace* parentNamespace = NULL;
			std::map<m::String, DeclNamespace*> nestedNamespace;
			std::map<m::String, DeclVariable> globals;
			std::map<m::String, DeclClass> classes;
			std::map<m::String, DeclFunction> functions;
		};
	}
}

#endif
