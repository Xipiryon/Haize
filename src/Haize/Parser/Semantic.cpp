#include <Muon/System/Log.hpp>

#include "Haize/Parser/Compiler.hpp"
#include "Haize/Parser/DeclStruct.hpp"

namespace
{
	/*
	* Custom SharedData
	* Specific for Semantic parsing
	*/
	struct InternalDataSemantic
	{
		hz::parser::DeclNamespace rootNamespace;
	};

	bool generateNode(InternalDataSemantic&, hz::parser::ASTNode* node);
}

namespace hz
{
	namespace parser
	{
		bool Compiler::semantic(Error& error)
		{
			// Skip empty AST
			if (m_nodeRoot->children->empty())
			{
				return true;
			}
			error.clear();
			error.step = Error::COMPILATION;

			InternalDataSemantic impl;
			impl.rootNamespace.declName = "#ROOT#";

			for(auto it = m_nodeRoot->children->begin(); it != m_nodeRoot->children->end(); ++it)
			{
				if(!generateNode(impl, *it))
				{
					error.state = Error::ERROR;
					return false;
				}
			}

			error.state = Error::SUCCESS;
			return true;
		}
	}
}

namespace
{
	bool generateNode(InternalDataSemantic& impl, hz::parser::ASTNode* node)
	{
		printf("Node: %s\n", hz::parser::TokenTypeStr[node->type]);
		switch(node->type)
		{
			default:
			break;
		}
		return true;
	}
}