
-- Executable
-------------------------------------------

project "HaizeExecutable"
	local ProjectRoot = os.getcwd()
	
	dependson("HaizeLibrary")
	language "C++"
	targetname "Haize"
	targetdir (SolutionRoot.."/bin")
	kind "ConsoleApp"
	
	if os.is("windows") then
		-- Because on Windows, you can't start a program if .dll are not in the same folder...
		postbuildcommands { string.gsub("copy "..SolutionRoot.."/bin/lib/*.dll "..SolutionRoot.."/bin/", "/", "\\") }
	end

	files	{
		ProjectRoot.."/main/main.cpp"
	}

	filter "Debug*"
		links	{ "Haize-d", "Muon-d" }

	filter "Release*"
		links { "Haize", "Muon" }
