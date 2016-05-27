#ifndef INCLUDE_HAIZE_DECLSTRUCT_INCLUDED
#define INCLUDE_HAIZE_DECLSTRUCT_INCLUDED

#include <map>
#include <Muon/String.hpp>
#include "Haize/Runtime/Variant.hpp"
#include "Haize/Runtime/ByteCode.hpp"

namespace hz
{
	namespace parser
	{
		struct ASTNodeFunctionDecl;
		struct ASTNodeFunctionCall;
	}

	namespace symbol
	{
		struct HAIZE_API Variable
		{
			union
			{
				bool global;
				m::u32 prefix;
			};
			m::String type;
			m::String name;
			Variant value;
		};

		struct HAIZE_API Function
		{
			union
			{
				m::u32 addrOffset;
				ByteCode* addrPtr;
			};
			m::String type;
			m::String name;
			std::vector<Variable> params;
		};

		struct HAIZE_API Class
		{
			m::String name;
			std::map<m::String, Function> methods;
			std::map<m::String, Variable> members;
		};

		struct HAIZE_API Namespace
		{
			static const m::String g_GlobalNamespaceName;
			m::String name;
			Namespace* parentNamespace = NULL;
			std::map<m::String, Namespace*> nestedNamespace;
			std::map<m::String, Variable> globals;
			std::map<m::String, Class> classes;
			std::map<m::String, Function> functions;
		};
	}
}

#endif
