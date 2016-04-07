
-- Executable
-------------------------------------------

project "HaizeExecutable"
	local ProjectRoot = os.getcwd()
	
	dependson("Haize")
	language "C++"
	targetname "Haize"
	targetdir (SolutionRoot.."/bin")
	kind "ConsoleApp"

	files	{
		ProjectRoot.."/main/main.cpp"
	}

	filter "Debug*"
		links	{ "Haize-d", "Muon-d" }

	filter "Release*"
		links { "Haize", "Muon" }
