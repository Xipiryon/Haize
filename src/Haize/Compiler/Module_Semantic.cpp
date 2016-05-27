#include <Muon/System/Log.hpp>

#include "Haize/Module.hpp"
#include "Haize/Compiler/DeclStruct.hpp"

namespace
{
	struct GenCodeInfo
	{
		m::u32 bcOffset;
		m::String type;
	};

	struct InternalDataSemantic
	{
		InternalDataSemantic(hz::Error& err, hz::symbol::Namespace& root)
			: error(err)
			, rootNamespace(root)
		{
			currNamespace = &rootNamespace;
		}

		hz::Error& error;
		hz::symbol::Namespace& rootNamespace;

		std::vector<hz::parser::ASTNode*> nodeToGenerate;
		hz::symbol::Namespace* currNamespace;

		std::vector<GenCodeInfo> genCodeInfo;
		std::vector<hz::ByteCode> byteCode;
	};

	bool findFunctionDecl(InternalDataSemantic& impl, const m::String& name, hz::symbol::Function** outFuncDecl);

	bool buildSymbolTable(InternalDataSemantic& impl, hz::parser::ASTNode* node);
	bool buildSTFunctionDecl(InternalDataSemantic& impl, hz::parser::ASTNode* node);

	bool generateCode(InternalDataSemantic& impl, hz::parser::ASTNode* node);
	// Declarations
	bool generateDeclNamespace(InternalDataSemantic& impl, hz::parser::ASTNode* node);
	bool generateDeclClass(InternalDataSemantic& impl, hz::parser::ASTNode* node);
	bool generateDeclFunction(InternalDataSemantic& impl, hz::parser::ASTNode* node);
	// Non Terminal nodes
	bool generateNTFuncCall(InternalDataSemantic& impl, hz::parser::ASTNode* node);
	// Terminal nodes
	bool generateTConstant(InternalDataSemantic& impl, hz::parser::ASTNode* node);
	bool generateTIdentifier(InternalDataSemantic& impl, hz::parser::ASTNode* node);
}

namespace hz
{
	bool Module::semantic(Error& error)
	{
		// Skip empty AST
		if (m_nodeRoot->children->empty())
		{
			return true;
		}
		error.clear();
		error.step = Error::COMPILATION;

		InternalDataSemantic impl(error, m_rootNamespace);
		// Build SymbolTable
		for (auto it = m_nodeRoot->children->begin(); it != m_nodeRoot->children->end(); ++it)
		{
			if (!buildSymbolTable(impl, *it))
			{
				error.state = Error::ERROR;
				return false;
			}
		}

		// Generate code for every required node
		for (auto it = impl.nodeToGenerate.begin(); it != impl.nodeToGenerate.end(); ++it)
		{
			if (!generateCode(impl, *it))
			{
				error.state = Error::ERROR;
				return false;
			}
		}

		m::u32 bcs = impl.byteCode.size();
		if (bcs > 0)
		{
			m_bytecode.reserve(m_bytecode.size() + bcs);
			m_bytecode.insert(m_bytecode.end(), impl.byteCode.begin(), impl.byteCode.end());
		}

		error.state = Error::SUCCESS;
		return true;
	}
}

namespace
{
	bool findFunctionDecl(InternalDataSemantic& impl, const m::String& name, hz::symbol::Function** outFuncDecl)
	{
		bool found = false;
		bool search = true;
		hz::symbol::Namespace* searchNamespace = impl.currNamespace;
		while (!found && search)
		{
			auto it = searchNamespace->functions.find(name);
			if (it != searchNamespace->functions.end())
			{
				found = true;
				*outFuncDecl = &it->second;
			}

			searchNamespace = searchNamespace->parentNamespace;
			search = (searchNamespace != NULL);
		}

		return found;
	}

	//********************
	// SYMBOL TABLE
	//********************
	bool buildSymbolTable(InternalDataSemantic& impl, hz::parser::ASTNode* node)
	{
		bool ok = true;
		switch (node->type)
		{
			case hz::parser::NT_FUNCTION_DECL:
				ok = buildSTFunctionDecl(impl, node);
				break;
			default:
				MUON_ERROR("Not implemented '%s'", hz::parser::TokenTypeStr[node->type]);
				break;
		}

		return ok;
	}

	bool buildSTFunctionDecl(InternalDataSemantic& impl, hz::parser::ASTNode* node)
	{
		hz::symbol::Function decl = ((hz::parser::ASTNodeFunctionDecl*)node)->decl;
		// Check there is no already defined function in the current namespace
		auto it = impl.currNamespace->functions.find(decl.name);
		if (it != impl.currNamespace->functions.end())
		{
			impl.error.message = "Function \"" + node->toString() + "\" already defined in "
				+ (impl.currNamespace == &impl.rootNamespace ? "global namespace" : impl.currNamespace->name);
			return false;
		}

		impl.currNamespace->functions[decl.name] = decl;
		impl.nodeToGenerate.push_back(node);
		return true;
	}

	//********************
	// CODE GENERATION
	//********************
	bool generateCode(InternalDataSemantic& impl, hz::parser::ASTNode* node)
	{
		bool ok = true;
		switch (node->type)
		{
			case hz::parser::NT_NAMESPACE_DECL:
				generateDeclNamespace(impl, node);
				break;
			case hz::parser::NT_CLASS_DECL:
				generateDeclClass(impl, node);
				break;
			case hz::parser::NT_FUNCTION_DECL:
				ok = generateDeclFunction(impl, node);
				break;
			default:
				MUON_ERROR("Not implemented '%s'", hz::parser::TokenTypeStr[node->type]);
				break;
		}

		return ok;
	}

	bool generateDeclNamespace(InternalDataSemantic& impl, hz::parser::ASTNode* node)
	{
		return true;
	}

	bool generateDeclClass(InternalDataSemantic& impl, hz::parser::ASTNode* node)
	{
		return true;
	}

	bool generateDeclFunction(InternalDataSemantic& impl, hz::parser::ASTNode* node)
	{
		hz::parser::ASTNodeFunctionDecl* func = (hz::parser::ASTNodeFunctionDecl*)node;
		func->decl.addrOffset = impl.byteCode.size();

		bool ok = true;
		for (auto it = func->children->begin(); it != func->children->end(); it++)
		{
			hz::parser::ASTNode* expr = *it;
			switch (expr->type)
			{
				case hz::parser::NT_EXPR_FUNC_CALL:
					ok = generateNTFuncCall(impl, expr);
					break;
				default:
					MUON_ERROR("Not implemented '%s'", hz::parser::TokenTypeStr[expr->type]);
					ok = false;
					break;
			}
			if (!ok)
			{
				return false;
			}
		}

		return true;
	}

	//********************
	// EXPRESSION GENERATION
	//********************

	bool generateNTFuncCall(InternalDataSemantic& impl, hz::parser::ASTNode* node)
	{
		hz::parser::ASTNodeFunctionCall* callNode = (hz::parser::ASTNodeFunctionCall*)node;
		m::u32 nbArg = callNode->children->size();

		// Generate code for each of its children (arguments)
		bool ok = true;
		for (auto it = callNode->children->begin(); it != callNode->children->end(); it++)
		{
			hz::parser::ASTNode* expr = *it;
			switch (expr->type)
			{
				case hz::parser::V_NIL:
				case hz::parser::V_TRUE:
				case hz::parser::V_FALSE:
				case hz::parser::V_NUMBER:
				case hz::parser::V_STRING:
					ok = generateTConstant(impl, expr);
					break;
				case hz::parser::V_IDENTIFIER:
					ok = generateTIdentifier(impl, expr);
					break;
				default:
					MUON_ERROR("Not implemented '%s'", hz::parser::TokenTypeStr[expr->type]);
					ok = false;
					break;
			}
			if (!ok)
			{
				return false;
			}
		}

		// Now we have all
		hz::symbol::Function* decl;
		if (!findFunctionDecl(impl, callNode->name, &decl))
		{
			impl.error.message = "Couldn't find any function \"" + callNode->toString() + "\"";
			return false;
		}

		//for (auto it = call->decl.params.begin(); it != call->decl.params.end(); it++)
		{
		}

		return true;
	}

	bool generateTConstant(InternalDataSemantic& impl, hz::parser::ASTNode* node)
	{
		GenCodeInfo info;
		hz::ByteCode bytecode;
		hz::Variant& var = ((hz::parser::ASTNodeConstant*)node)->decl.value;
		bool isString = node->type == hz::parser::V_STRING;
		m::u16 constSize = (isString ? var.get<m::String>().size() : var.size());
		m::u16 requiredByteCode = constSize / sizeof(m::u32) + 1;

		// Push directly inside the byte code
		bytecode.setup(hz::SYS_W_CONST, (m::u8)node->type, constSize);
		impl.byteCode.push_back(bytecode);

		// Write down the bytes
		std::vector<hz::ByteCode> dataByteCode(requiredByteCode);
		void* dataSrc = (isString ? (void*)var.get<m::String>().cStr() : var.get<void*>());
		void* dataDst = &dataByteCode[0].data;
		::memcpy(dataDst, dataSrc, constSize);
		impl.byteCode.insert(impl.byteCode.end(), dataByteCode.begin(), dataByteCode.end());
		return true;
	}

	bool generateTIdentifier(InternalDataSemantic& impl, hz::parser::ASTNode* node)
	{
		GenCodeInfo info;

		return true;
	}
}
