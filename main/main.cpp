
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>
#include <iostream>

#include <Muon/Core/Define.hpp>
#include <Muon/System/Log.hpp>
#include "Haize/VM.hpp"

void leave(int code)
{
	muon::system::Log::close();

#ifdef MUON_PLATFORM_WINDOWS
	::system("PAUSE");
#endif
	::exit(code);
}

void displayUsage()
{
	muon::system::Log log("", muon::LOG_INFO);
#define HAIZE_USAGE_LOG(Option, Text) log() << " " Option << " \t " Text << muon::endl << muon::endl
	HAIZE_USAGE_LOG("-i", "Set the mode as 'interactive'." << muon::endl
					<< "\t It will compile on-the-fly any given text input." << muon::endl
					<< "\t This option override any other option." << muon::endl
					<< "\t Type 'return 0' to exit interactive mode.");
	HAIZE_USAGE_LOG("-e EXPR", "Evaluate the expression EXPR and return.");
	HAIZE_USAGE_LOG("-f FILE", "Load and evaluate the content located in FILE and return.");
	HAIZE_USAGE_LOG("-b FILE", "Load and evaluate the bytecode content located in FILE and return.");
	HAIZE_USAGE_LOG("-o", "Export generated bytecode to 'FILE.hzc'." << muon::endl
					<< "\t Works only with -f or -e option.");
#undef HAIZE_USAGE_LOG
}

namespace
{
	bool g_Interactive = false;
	bool g_Evaluate = false;
	bool g_LoadFile = false;
	bool g_LoadBytecode = false;
	bool g_ExportBytecode = false;
	muon::String g_Expr;
	muon::String g_Filename;
	char* g_Buffer;
}

bool parseArguments(int, char**);
void executeProgram();

int main(int argc, char** argv)
{
	if(!muon::meta::MetaDatabase::isInstantiated())
	{
		muon::meta::MetaDatabase::instantiate();
	}
	muon::system::Log::registerDefaultLogImpl();
	muon::system::Log log("", muon::LOG_INFO);
	log << " => Haize v" << HAIZE_VERSION << " (c) 2015" << muon::endl << muon::endl;

	if(parseArguments(argc, argv))
	{
		executeProgram();
	}
	else
	{
		displayUsage();
	}

	leave(0);
}

bool parseArguments(int argc, char** argv)
{
	muon::system::Log log("Main", muon::LOG_INFO);
	bool validArgument = false;

	for(int i = 0; i < argc; ++i)
	{
		// INTERACTIVE MODE
		if(!strcmp(argv[i], "-i"))
		{
			g_Interactive = true;
			return true;
		}
		// EVAL EXPR
		else if(!strcmp(argv[i], "-e"))
		{
			if(++i < argc)
			{
				g_Expr += argv[i];
			}
			else
			{
				log(muon::LOG_ERROR) << "Missing EXPR after -e argument (use quotes)!" << muon::endl;
			}
			g_Evaluate = true;
			validArgument = true;
		}
		// EXPORT BYTECODE
		else if (!strcmp(argv[i], "-o"))
		{
			g_ExportBytecode = true;
		}
		// LOAD FILE
		else if (!strcmp(argv[i], "-f"))
		{
			if (++i < argc)
			{
				g_Filename = argv[i];
				g_LoadFile = true;
				validArgument = true;
			}
			else
			{
				log(muon::LOG_ERROR) << "Missing FILE after -f argument!" << muon::endl;
			}
		}
		// BYTECODE LOAD FILE
		else if (!strcmp(argv[i], "-b"))
		{
			if (++i < argc)
			{
				g_Filename = argv[i];
				std::ifstream file(g_Filename.cStr());
				if (!file)
				{
					log(muon::LOG_ERROR) << "Couldn't load: " << g_Filename << muon::endl;
					return false;
				}
				if (!file.eof())
				{
					file.seekg(0, std::ios::end);
					muon::u64 length = (muon::u64)file.tellg();
					file.seekg(0, std::ios::beg);
					log(muon::LOG_INFO) << "Reading " << length << " bytes ..." << muon::endl;
					g_Buffer = (char*)malloc(sizeof(char) * (size_t)length);
					file.read(g_Buffer, length);
					log(muon::LOG_INFO) << "... done !" << muon::endl;
					validArgument = true;
					g_LoadBytecode = true;
				}
			}
			else
			{
				log(muon::LOG_ERROR) << "Missing FILE after -b argument!" << muon::endl;
			}
		}
	}

	return validArgument;
}

void executeProgram()
{
	muon::system::Log log("Haize", muon::LOG_INFO);
	hz::VMInstance vm;

	if(g_Interactive)
	{
		bool returned = false;
		do
		{
			std::string line;
			std::getline(std::cin, line);
			//hz::BoxedValue r = hz::eval(vm, line.c_str());
			//returned = (r.getType() != hz::BoxedValue::NIL && r.getInteger() == 0);
			returned = true;
		}
		while(!returned);
	}
	else if(g_Evaluate)
	{
		log() << "> Parsing \"" << g_Expr << "\"" << muon::endl;
		if (vm.eval(g_Expr.cStr()))
		{
			if (g_ExportBytecode)
			{
				muon::String outputFilename = "eval.bytecode";
				log() << "Outputting to \"" << outputFilename << "\"" << muon::endl;
				std::ofstream file(outputFilename.cStr(), std::ios::binary | std::ios::trunc);
				file.write((const char*)vm.getInfo().IRCode, vm.getInfo().IRCodeSize);
				file.close();
			}
		}

		return;
	}
	// LOAD FILE
	else if (g_LoadFile)
	{
		muon::String module = "RuntimeEval";
		log() << "> Parsing \"" << g_Filename << "\"" << muon::endl;
		if (!vm.load(g_Filename.cStr()))
		{
			log(muon::LOG_ERROR) << "Couldn't load " << g_Filename << "!" << muon::endl;
			return;
		}

		if (!vm.compile(module))
		{
			log(muon::LOG_ERROR) << "Couldn't compile " << g_Filename << "!" << muon::endl;
			return;
		}

		if (!vm.execute(module))
		{
			log(muon::LOG_ERROR) << "Couldn't execute " << g_Filename << "!" << muon::endl;
			return;
		}

		if (g_ExportBytecode)
		{
			muon::String outputFilename = g_Filename + "c";
			log() << "Outputting to \"" << outputFilename << "\"" << muon::endl;
			std::ofstream file(outputFilename.cStr(), std::ios::binary | std::ios::trunc);
			file.write((const char*)vm.getInfo().IRCode, vm.getInfo().IRCodeSize);
			file.close();
		}
		return;
	}
	// BYTECODE LOAD FILE
	else if (g_LoadBytecode)
	{
		log() << "> Parsing \"" << g_Filename << "\"" << muon::endl;
		// TODO
		free(g_Buffer);
		return;
	}
}