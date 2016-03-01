
#include <fstream>

#include <Muon/System/Log.hpp>
#include <Muon/System/Time.hpp>
#include "Haize/Engine.hpp"

#include "tinyxml2.h"

namespace
{
	const muon::u32 FileContentBuffer = 4096;
}

bool loadFile(const char* filename, char* outbuffer)
{
	muon::system::Log log("LoadFile");

	std::ifstream file(filename);
	if (!file)
	{
		log(muon::LOG_ERROR) << "Couldn't load: " << filename << muon::endl;
		return false;
	}
	if (!file.eof())
	{
		file.seekg(0, std::ios::end);
		muon::u64 length = (muon::u64)file.tellg();
		file.seekg(0, std::ios::beg);
		log(muon::LOG_INFO) << "Reading " << length << " bytes from \"" << filename << "\"..." << muon::endl;
		MUON_ASSERT_BREAK(length < FileContentBuffer, "File buffer is not big enough!");
		file.read(outbuffer, length);
		log(muon::LOG_INFO) << "... done !" << muon::endl;
	}
	return true;
}

int main(int argc, char** argv)
{
	if(!muon::meta::MetaDatabase::isInstantiated())
	{
		muon::meta::MetaDatabase::createInstance();
	}
	muon::system::Log::registerDefaultLogImpl();
	muon::system::Log mainLog("Main", muon::LOG_INFO);

	mainLog() << "Number of arguments: " << argc << muon::endl;
	for (muon::i32 i = 0; i < argc; ++i)
	{
		mainLog() << "\t: " << argv[i] << muon::endl;
	}

	// Required variables
	muon::system::Time clockTest;
	muon::String title;
	muon::u32 errorCount = 0;
	muon::u32 totalTests = 0;
	tinyxml2::XMLDocument xmlDoc;
	tinyxml2::XMLElement* xmlRoot = xmlDoc.NewElement("testsuite");

#define HAIZE_TITLE(msg) do { mainLog() << msg << muon::endl; title = msg; } while(false);
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
	muon::String file;
	muon::String module;

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

		//HAIZE_TITLE(muon::String::join("Evaluating Script: \"", eval, "\""));
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

	// Expression
	{
		module = "Expr";
		file = "unittests/scripts/expressions_fail.hz";

		hz::Context* context = vm.createContext(module.cStr());
		HAIZE_TITLE("Checking 'Expression' script");
		bool ok;

		ok = context->load(file.cStr());
		infoError = context->getLastError();
		HAIZE_CHECK(ok, "Load Error (\"%s\") [%d:%d] %s", file.cStr(), infoError.line, infoError.column, infoError.message.cStr());

		// Explicit failing test
		ok = context->compile();
		HAIZE_CHECK(!ok, "Compilation should have failed because of \"free code\" (\"%s\")", file.cStr());

		// As compilation should have failed, don't even try to execute it, as it is not loaded inside Context
		// TODO?
	}

	// Functions
	{
		module = "Function";
		file = "unittests/scripts/functions.hz";

		hz::Context* context = vm.createContext(module.cStr());
		HAIZE_TITLE("Checking 'Functions' script");
		bool ok;

		ok = context->load(file.cStr());
		infoError = context->getLastError();
		HAIZE_CHECK(ok, "[LOADING] Error in step \"%s\" (\"%s\") [%d:%d] %s", stepStr[infoError.step], file.cStr(), infoError.line, infoError.column, infoError.message.cStr());

		ok = context->compile();
		infoError = context->getLastError();
		HAIZE_CHECK(ok, "[COMPILATION] Error in step \"%s\" (\"%s\") [%d:%d] %s", stepStr[infoError.step], file.cStr(), infoError.line, infoError.column, infoError.message.cStr());

		//TODO:
		//Preparation

		//TODO:
		//ok = context->execute();
		//infoError = context->getLastError();
		//HAIZE_CHECK(ok, "[EXECUTION] Error in step \"%s\" (\"%s\") [%d:%d] %s", stepStr[infoError.step], file.cStr(), infoError.line, infoError.column, infoError.message.cStr());
	}

	// END UNIT TEST
	// ***************
	mainLog(errorCount == 0 ? muon::LOG_INFO : muon::LOG_ERROR) << "Error Count: " << errorCount << muon::endl;

	xmlRoot->SetAttribute("tests", totalTests);
	xmlDoc.InsertFirstChild(xmlRoot);
	xmlDoc.SaveFile("unittests.xml");

	muon::system::Log::close();

	return -((muon::i32)errorCount);
}
