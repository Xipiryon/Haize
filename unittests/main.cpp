
#include <Muon/System/Log.hpp>
#include "Haize/VM.hpp"

int main(int argc, char** argv)
{
	if(!muon::meta::MetaDatabase::isInstantiated())
	{
		muon::meta::MetaDatabase::instantiate();
	}
	muon::system::Log::registerDefaultLogImpl();
	hz::VMInstance vm;

	muon::system::Log::close();
	return 0;
}
