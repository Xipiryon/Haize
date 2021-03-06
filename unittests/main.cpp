#include <fstream>

#include <Muon/System/Log.hpp>
#include <Muon/System/Time.hpp>
#include "Haize/Engine.hpp"

#include "tinyxml2.h"

int main(int argc, char** argv)
{
	m::system::Log::registerDefaultLogImpl();
	m::system::Log mainLog("Main", m::LOG_INFO);

	mainLog() << "Number of arguments: " << argc << m::endl;
	for (m::i32 i = 0; i < argc; ++i)
	{
		mainLog() << "\t: " << argv[i] << m::endl;
	}

	// Required variables
	m::system::Time clockTest;
	m::String title;
	m::u32 errorCount = 0;
	m::u32 totalTests = 0;
	tinyxml2::XMLDocument xmlDoc;
	tinyxml2::XMLElement* xmlRoot = xmlDoc.NewElement("testsuite");

#define HAIZE_TITLE(msg) do { mainLog() << msg << m::endl; title = msg; } while(false);
#define HAIZE_NODE_BEGIN(cond)	++totalTests; tinyxml2::XMLElement* xmlNode = xmlDoc.NewElement("testcase"); \
									xmlNode->SetAttribute("name", #cond); \
									xmlNode->SetAttribute("classname", title.cStr()); \
									xmlNode->SetAttribute("time", clockTest.now()*1000);
#define HAIZE_NODE_ERR(err)		++errorCount; tinyxml2::XMLElement* xmlErr = xmlDoc.NewElement("failure"); xmlErr->SetText(err); xmlNode->InsertEndChild(xmlErr);
#define HAIZE_NODE_END			xmlRoot->InsertEndChild(xmlNode); clockTest.start();
#if defined(MUON_PLATFORM_WINDOWS)
#	define HAIZE_CHECK(cond, err, ...)  do { HAIZE_NODE_BEGIN(cond) if(!(cond)) { HAIZE_NODE_ERR(err); MUON_ERROR("\t-> " err, __VA_ARGS__);} HAIZE_NODE_END } while(false);
#else
#	define HAIZE_CHECK(cond, err, args...) do { HAIZE_NODE_BEGIN(cond) if(!(cond)) { HAIZE_NODE_ERR(err); MUON_ERROR("\t-> " err, ##args);} HAIZE_NODE_END } while(false);
#endif

	// ***************
	// BEGIN UNIT TEST
	clockTest.start();

	hz::Engine& vm = hz::Engine::getInstance();
	hz::Error infoError;
	m::String file;
	m::String module;

	const char* stepStr[] = {
		"LOADING",
		"COMPILATION",
		"PREPARATION",
		"EXECUTION"
	};

	// Eval some scripts
	{
		// The eval line must be set this way, so we have correct line ending
		const char* eval = MUON_STR(
			// declare a variable
			a = 0;
		// declare function
		function foo(a)
		{
			print(a);
		}
		foo(a);
		return 64;
		);

		//HAIZE_TITLE(m::String::join("Evaluating Script: \"", eval, "\""));
		//HAIZE_CHECK(vm.eval(eval), "VM.eval() function failed!");
	}

	// Module Creation / Destruction
	{
		HAIZE_TITLE("Context creation / destruction");
		module = "CreateDestroy";
		hz::Context* context = vm.createContext(module);
		HAIZE_CHECK(context != NULL, "Context creation failed!");
		bool removed = vm.destroyContext(module);
		HAIZE_CHECK(removed, "Context destruction failed!");
		context = vm.getContext(module);
		HAIZE_CHECK(context == NULL, "Returning non-null destroyed context!");
		// Recreate
		vm.createContext(module);
		context = vm.getContext(module);
		HAIZE_CHECK(context != NULL, "Returning already-created context failed!");
		// Clean memory
		vm.destroyContext(module);
	}

	struct FileModule
	{
		m::String file;
		m::String module;
	};
	m::i32 scriptIndex = 0;

	// ************************
	// Test that should work :)
	FileModule scriptSuccessTests[] = {
		{ "unittests/scripts/functions_noArg_noRet.hz", "Function: No Arguments, No Return" },
		{ "unittests/scripts/functions_Arg_noRet.hz", "Function: Arguments, No Return" },
		{ "unittests/scripts/functions_noArg_Ret.hz", "Function: No Arguments, Return" },
		{ "unittests/scripts/functions_Arg_Ret.hz", "Function: Arguments, Return" },

		{ "unittests/scripts/expr_Comments.hz", "Comments" },
		{ "unittests/scripts/expr_Unop.hz", "Unary Operator" },
		{ "unittests/scripts/expr_Binop.hz", "Binary Operator" },
		{ "unittests/scripts/expr_Namespace.hz", "Namespaces" },
		{ "unittests/scripts/expr_Global.hz", "Global Declaration" },
		{ "unittests/scripts/expr_VarNewDel.hz", "Variable Declaration" },
		{ "unittests/scripts/expr_Accessor.hz", "Class and Array member accessor" },

		{ "unittests/scripts/class_Empty.hz", "Empty Class" },
		{ "unittests/scripts/class_Member.hz", "Class with Member" },
		{ "unittests/scripts/class_Function.hz", "Class with Function" },
		{ "unittests/scripts/class_Full.hz", "Class with both Member and Function" },

		{ "unittests/scripts/logic_if.hz", "Control Flow: if" },
		{ "unittests/scripts/logic_for.hz", "Control Flow: for" },
		{ "unittests/scripts/logic_while.hz", "Control Flow: while" },
		{ "unittests/scripts/logic_switch.hz", "Control Flow: switch" },

		{ "", "" }
	};

	for (scriptIndex = 0; scriptSuccessTests[scriptIndex].file != ""; ++scriptIndex)
	{
		module = scriptSuccessTests[scriptIndex].module;
		file = scriptSuccessTests[scriptIndex].file;

		hz::Context* context = vm.createContext(module);
		HAIZE_TITLE("Checking '" + module + "' script");
		bool ok;

		ok = context->load(file);
		infoError = context->getLastError();
		HAIZE_CHECK(ok, "[%s] Error in section \"%s\" [%d:%d] %s", stepStr[infoError.step], infoError.section.cStr(), infoError.line, infoError.column, infoError.message.cStr());
		if (!ok) continue;

		ok = context->compile();
		infoError = context->getLastError();
		HAIZE_CHECK(ok, "[%s] Error in section \"%s\" [%d:%d] %s", stepStr[infoError.step], infoError.section.cStr(), infoError.line, infoError.column, infoError.message.cStr());
		if (!ok) continue;

		//TODO:
		//Preparation
		if (!ok) continue;

		//TODO:
		//ok = context->execute();
		//infoError = context->getLastError();
		//HAIZE_CHECK(ok, "[EXECUTION] Error in section \"%s\" [%d:%d] %s", stepStr[infoError.step], infoError.section.cStr(), infoError.line, infoError.column, infoError.message.cStr());

		vm.destroyContext(module);
	}

	// ************************
	// Multiple Pass
	{
		module = "Multiple Pass";
		m::String file_A = "unittests/scripts/multiple_LoadA.hz";
		m::String file_B = "unittests/scripts/multiple_LoadB.hz";

		hz::Context* context = vm.createContext(module);
		HAIZE_TITLE("Checking 'Multiple Load' script");
		bool ok;

		{
			// Load in multiple passes
			ok = context->load(file_A);
			infoError = context->getLastError();
			HAIZE_CHECK(ok, "[%s] Error in section \"%s\" [%d:%d] %s", stepStr[infoError.step], infoError.section.cStr(), infoError.line, infoError.column, infoError.message.cStr());
			if (!ok) goto skipMultiplePass;

			ok = context->load(file_B);
			infoError = context->getLastError();
			HAIZE_CHECK(ok, "[%s] Error in section \"%s\" [%d:%d] %s", stepStr[infoError.step], infoError.section.cStr(), infoError.line, infoError.column, infoError.message.cStr());
			if (!ok) goto skipMultiplePass;

			// Compile both
			ok = context->compile();
			infoError = context->getLastError();
			HAIZE_CHECK(ok, "[%s] Error in section \"%s\" [%d:%d] %s", stepStr[infoError.step], infoError.section.cStr(), infoError.line, infoError.column, infoError.message.cStr());
			if (!ok) goto skipMultiplePass;

			// Prepare function
			ok = context->prepare("main");
			infoError = context->getLastError();
			HAIZE_CHECK(ok, "[%s] Error in section \"%s\" [%d:%d] %s", stepStr[infoError.step], infoError.section.cStr(), infoError.line, infoError.column, infoError.message.cStr());
			if (!ok) goto skipMultiplePass;

			// Execute code
			ok = context->execute();
			infoError = context->getLastError();
			HAIZE_CHECK(ok, "[%s] Error in section \"%s\" [%d:%d] %s", stepStr[infoError.step], infoError.section.cStr(), infoError.line, infoError.column, infoError.message.cStr());
			if (!ok) goto skipMultiplePass;
		}
		{
			// Post compile
			HAIZE_TITLE("Checking 'Post Compile' script");
			m::String postCompile = "unittests/scripts/multiple_PostCompile.hz";

			// Load and compile more code in the same context
			ok = context->load(postCompile);
			infoError = context->getLastError();
			HAIZE_CHECK(ok, "[%s] Error in section \"%s\" [%d:%d] %s", stepStr[infoError.step], infoError.section.cStr(), infoError.line, infoError.column, infoError.message.cStr());
			if (!ok) goto skipMultiplePass;

			ok = context->compile();
			infoError = context->getLastError();
			HAIZE_CHECK(ok, "[%s] Error in section \"%s\" [%d:%d] %s", stepStr[infoError.step], infoError.section.cStr(), infoError.line, infoError.column, infoError.message.cStr());
			if (!ok) goto skipMultiplePass;

			//Prepare again
			ok = context->prepare("postCompile");
			infoError = context->getLastError();
			HAIZE_CHECK(ok, "[%s] Error in section \"%s\" [%d:%d] %s", stepStr[infoError.step], infoError.section.cStr(), infoError.line, infoError.column, infoError.message.cStr());
			if (!ok) goto skipMultiplePass;

			ok = context->execute();
			infoError = context->getLastError();
			HAIZE_CHECK(ok, "[%s] Error in section \"%s\" [%d:%d] %s", stepStr[infoError.step], infoError.section.cStr(), infoError.line, infoError.column, infoError.message.cStr());
		}
	skipMultiplePass:
		// Finaly clear everything
		vm.destroyContext(module);
	}

	// END UNIT TEST
	// ***************
	mainLog(errorCount == 0 ? m::LOG_INFO : m::LOG_ERROR) << "Error Count: " << errorCount << m::endl;

	xmlRoot->SetAttribute("tests", totalTests);
	xmlDoc.InsertFirstChild(xmlRoot);
	xmlDoc.SaveFile("unittests.xml");

	m::system::Log::close();

	return -((m::i32)errorCount);
}
