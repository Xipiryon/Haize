
-- Unit Tests
-------------------------------------------

project "UnitTests"
	local ProjectRoot = os.getcwd()

	dependson("Haize")
	language "C++"
	targetname "UnitTests"
	targetdir (SolutionRoot.."/bin/exe")
	kind "ConsoleApp"

	if os.is("windows") then
		postbuildcommands { string.gsub("copy "..SolutionRoot.."/bin/exe/UnitTests*.exe "..SolutionRoot.."/bin/", "/", "\\") }
	else
		postbuildcommands { "cp "..SolutionRoot.."/bin/exe/UnitTests* "..SolutionRoot.."/bin/" }
	end

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
