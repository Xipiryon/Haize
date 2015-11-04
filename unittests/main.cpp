
#include <Muon/System/Log.hpp>
#include "Haize/VM.hpp"

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

	// Simple file loading
	{
		HAIZE_TITLE(" ** File loading **");
	}

	muon::system::Log::close();
	return 0;
}
