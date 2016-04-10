
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

	filter "Debug*"
		links	{ "Haize-d", "Muon-d" }

	filter "Release*"
		links { "Haize-r", "Muon-r" }

	filter "Final*"
		links { "Haize-f", "Muon-f" }

	filter {}