
#include <fstream>

#include <Muon/System/Log.hpp>
#include <Muon/System/Time.hpp>
#include "Haize/VM.hpp"

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
		muon::meta::MetaDatabase::instantiate();
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

	hz::VMInstance vm;
	muon::String file;
	char buffer[FileContentBuffer];

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

		HAIZE_TITLE(muon::String::join("Evaluating Script: \"", eval, "\""));
		HAIZE_CHECK(vm.eval(eval), "VM.eval() function failed!");
	}

	// Expression
	{
		file = "unittests/scripts/expressions.hz";
		HAIZE_TITLE("Checking 'Expression' script");
		HAIZE_CHECK(loadFile(file.cStr(), buffer), "Couldn't load file\"%s\"!", file.cStr());
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
