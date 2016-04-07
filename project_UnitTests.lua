
-- Unit Tests
-------------------------------------------

project "UnitTests"
	local ProjectRoot = os.getcwd()

	dependson("Haize")
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

	filter "Debug*"
		links	{ "Haize-d", "Muon-d" }

	filter "Release*"
		links { "Haize", "Muon" }
