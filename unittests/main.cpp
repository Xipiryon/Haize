
#include <fstream>

#include <Muon/System/Log.hpp>
#include "Haize/VM.hpp"

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

	muon::u32 errorCount = 0;

#define HAIZE_TITLE(msg) mainLog() << msg << muon::endl
#if defined(MUON_PLATFORM_WINDOWS)
#	define HAIZE_CHECK(cond, err, ...) if(!(cond)) {++errorCount; MUON_ERROR("\t-> " err, __VA_ARGS__);}
#else
#	define HAIZE_CHECK(cond, err, args...) if(!(cond)) {++errorCount; MUON_ERROR("\t-> " err, ##args);}
#endif

	// ***************
	// BEGIN UNIT TEST

	hz::VMInstance vm;
	muon::String file;
	char buffer[FileContentBuffer];

	// Eval some scripts
	{

		HAIZE_TITLE(" ** Evaluating Script **");
		const char* eval =
		" // declare a variable "
		"a = 0;"
		" // declare function "
		"function foo(a) { print(a); }"
		"foo(a);"
		"return 64;";
		HAIZE_CHECK(vm.eval(eval), "VM.eval() function failed!");
	}

	// Expression
	{
		file = "unittests/scripts/expressions.hz";
		HAIZE_TITLE(" ** File loading: " << file << " **");
		HAIZE_CHECK(loadFile(file.cStr(), buffer), "Couldn't load file!");

	}

	// Function
	{
		file = "unittests/scripts/functions.hz";
		HAIZE_TITLE(" ** File loading: " << file << " **");
		HAIZE_CHECK(loadFile(file.cStr(), buffer), "Couldn't load file!");

	}

	// END UNIT TEST
	// ***************

	mainLog(errorCount == 0 ? muon::LOG_INFO : muon::LOG_ERROR) << "Error Count: " << errorCount << muon::endl;

	muon::system::Log::close();

	return -((muon::i32)errorCount);
}
