
-- Executable
-------------------------------------------

project "HaizeExecutable"
	local ProjectRoot = os.getcwd()
	
	dependson("HaizeLibrary")
	language "C++"
	targetname "Haize"
	targetdir (SolutionRoot.."/bin")
	kind "ConsoleApp"

	files	{
		ProjectRoot.."/main/main.cpp"
	}

	filter "Debug*"
		links	{ "Haize-d", "Muon-d" }
		defines { "HAIZE_DEBUG" }

	filter "Release*"
		links { "Haize", "Muon" }
