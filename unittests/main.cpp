
#include <fstream>

#include <Muon/System/Log.hpp>
#include <Muon/System/Time.hpp>
#include "Haize/Engine.hpp"

#include "tinyxml2.h"

namespace
{
	const m::u32 FileContentBuffer = 4096;
}

bool loadFile(const char* filename, char* outbuffer)
{
	m::system::Log log("LoadFile");

	std::ifstream file(filename);
	if (!file)
	{
		log(m::LOG_ERROR) << "Couldn't load: " << filename << m::endl;
		return false;
	}
	if (!file.eof())
	{
		file.seekg(0, std::ios::end);
		m::u64 length = (m::u64)file.tellg();
		file.seekg(0, std::ios::beg);
		log(m::LOG_INFO) << "Reading " << length << " bytes from \"" << filename << "\"..." << m::endl;
		MUON_ASSERT_BREAK(length < FileContentBuffer, "File buffer is not big enough!");
		file.read(outbuffer, length);
		log(m::LOG_INFO) << "... done !" << m::endl;
	}
	return true;
}

int main(int argc, char** argv)
{
	if(!m::meta::MetaDatabase::isInstantiated())
	{
		m::meta::MetaDatabase::createInstance();
	}
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

	hz::Engine& vm = hz::Engine::createInstance();
	hz::InfoError infoError;
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
			function foo(a) { print(a); }
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
		hz::Context* context = vm.createContext(module.cStr());
		HAIZE_CHECK(context != NULL, "Context creation failed!");
		bool removed = vm.destroyContext(module.cStr());
		HAIZE_CHECK(removed, "Context destruction failed!");
		context = vm.getContext(module.cStr());
		HAIZE_CHECK(context == NULL, "Returning non-null destroyed context!");
		// Recreate
		vm.createContext(module.cStr());
		context = vm.getContext(module.cStr());
		HAIZE_CHECK(context != NULL, "Returning already-created context failed!");
		// Clean memory
		vm.destroyContext(module.cStr());
	}

	// Free code
	{
		module = "Free Code";
		file = "unittests/scripts/freecode.hz";

		hz::Context* context = vm.createContext(module.cStr());
		HAIZE_TITLE("Checking 'Free Code' script");
		bool ok;

		ok = context->load(file.cStr());
		infoError = context->getLastError();
		HAIZE_CHECK(ok, "[%s] Error in section \"%s\" [%d:%d] %s", stepStr[infoError.step], infoError.section.cStr(), infoError.line, infoError.column, infoError.message.cStr());

		// Explicit failing test
		ok = context->compile();
		HAIZE_CHECK(!ok, "[%s] Should have failed because of \"free code\"!");

		// As compilation should have failed, don't even try to execute it, as it is not loaded inside Context
		// TODO?
		vm.destroyContext(module.cStr());
	}
	
	// Comments
	{
		module = "Comments";
		file = "unittests/scripts/comments.hz";

		hz::Context* context = vm.createContext(module.cStr());
		HAIZE_TITLE("Checking 'Comments' script");
		bool ok;

		ok = context->load(file.cStr());
		infoError = context->getLastError();
		HAIZE_CHECK(ok, "[%s] Error in section \"%s\" [%d:%d] %s", stepStr[infoError.step], infoError.section.cStr(), infoError.line, infoError.column, infoError.message.cStr());

		ok = context->compile();
		HAIZE_CHECK(ok, "[%s] Error in section \"%s\" [%d:%d] %s", stepStr[infoError.step], infoError.section.cStr(), infoError.line, infoError.column, infoError.message.cStr());
		vm.destroyContext(module.cStr());
	}

	// Multiple Load
	{
		module = "MultipleLoad";
		const char* file_A = "unittests/scripts/multipleLoad_A.hz";
		const char* file_B = "unittests/scripts/multipleLoad_B.hz";

		hz::Context* context = vm.createContext(module.cStr());
		HAIZE_TITLE("Checking 'Multiple Load' script");
		bool ok;

		ok = context->load(file_A);
		infoError = context->getLastError();
		HAIZE_CHECK(ok, "[%s] Error in section \"%s\" [%d:%d] %s", stepStr[infoError.step], infoError.section.cStr(), file_A, infoError.line, infoError.column, infoError.message.cStr());
		ok = context->load(file_B);
		infoError = context->getLastError();
		HAIZE_CHECK(ok, "[%s] Error in section \"%s\" [%d:%d] %s", stepStr[infoError.step], infoError.section.cStr(), file_B, infoError.line, infoError.column, infoError.message.cStr());

		ok = context->compile();
		infoError = context->getLastError();
		HAIZE_CHECK(ok, "[%s] Error in section \"%s\" [%d:%d] %s", stepStr[infoError.step], infoError.section.cStr(), infoError.line, infoError.column, infoError.message.cStr());

		//TODO:
		//Preparation

		//TODO:
		//ok = context->execute();
		//infoError = context->getLastError();
		//HAIZE_CHECK(ok, "[EXECUTION] Error in section \"%s\" [%d:%d] %s", stepStr[infoError.step], infoError.section.cStr(), infoError.line, infoError.column, infoError.message.cStr());
		vm.destroyContext(module.cStr());
	}

	// Functions
	struct FileModule
	{
		m::String file;
		m::String module;
	} scriptTests[] = {
		{"unittests/scripts/functions_noArg_noRet.hz", "Function: No Arguments, No Return"},
		{"unittests/scripts/functions_Arg_noRet.hz", "Function: Arguments, No Return"},
		{"unittests/scripts/functions_noArg_Ret.hz", "Function: No Arguments, Return"},
		{"unittests/scripts/functions_Arg_Ret.hz", "Function: Arguments, Return"},

		{"",""}
	};
	m::i32 scriptIndex = 0;

	while(scriptTests[scriptIndex].file != "")
	{
		module = scriptTests[scriptIndex].module;
		file = scriptTests[scriptIndex].file;

		hz::Context* context = vm.createContext(module.cStr());
		HAIZE_TITLE("Checking '" + module + "' script");
		bool ok;

		ok = context->load(file.cStr());
		infoError = context->getLastError();
		HAIZE_CHECK(ok, "[%s] Error in section \"%s\" [%d:%d] %s", stepStr[infoError.step], infoError.section.cStr(), infoError.line, infoError.column, infoError.message.cStr());

		ok = context->compile();
		infoError = context->getLastError();
		HAIZE_CHECK(ok, "[%s] Error in section \"%s\" [%d:%d] %s", stepStr[infoError.step], infoError.section.cStr(), infoError.line, infoError.column, infoError.message.cStr());

		//TODO:
		//Preparation

		//TODO:
		//ok = context->execute();
		//infoError = context->getLastError();
		//HAIZE_CHECK(ok, "[EXECUTION] Error in section \"%s\" [%d:%d] %s", stepStr[infoError.step], infoError.section.cStr(), infoError.line, infoError.column, infoError.message.cStr());
		++scriptIndex;
		vm.destroyContext(module.cStr());
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
