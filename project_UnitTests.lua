
-- Unit Tests
-------------------------------------------

project "Haize_UnitTests"
	local ProjectRoot = os.getcwd()

	dependson("Haize_Core")
	language "C++"
	targetname "UnitTests"
	targetdir (SolutionRoot.."/bin")
	kind "ConsoleApp"

	files	{
		ProjectRoot.."/unittests/main.cpp",
		ProjectRoot.."/unittests/tinyxml2.cpp",
		ProjectRoot.."/unittests/tinyxml2.h",
		ProjectRoot.."/unittests/**.hz"
	}

	links	{ "Haize_Core", "Muon_Core" }

	filter {}