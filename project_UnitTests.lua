
project "UnitTests"
	language "C++"
	targetname "UnitTests"
	targetdir (SolutionRoot.."/bin")
	kind "ConsoleApp"

	files	{
		ProjectRoot.."/unittests/main.cpp"
	}

	filter "Debug*"
		links	{ "Haize-d", "Muon-d" }

	filter "Release*"
		links { "Haize", "Muon" }
