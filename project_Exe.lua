
-- Executable
-------------------------------------------

project "HaizeExecutable"
	local ProjectRoot = os.getcwd()
	
	dependson("Haize")
	language "C++"
	targetname "Haize"
	targetdir (SolutionRoot.."/bin/exe")
	kind "ConsoleApp"

	if os.is("windows") then
		postbuildcommands { string.gsub("copy "..SolutionRoot.."/bin/exe/Haize*.exe "..SolutionRoot.."/bin/", "/", "\\") }
	else
		postbuildcommands { "cp "..SolutionRoot.."/bin/exe/Haize* "..SolutionRoot.."/bin/" }
	end

	files	{
		ProjectRoot.."/main/main.cpp"
	}

	filter "Debug*"
		links	{ "Haize-d", "Muon-d" }

	filter "Release*"
		links { "Haize", "Muon" }
