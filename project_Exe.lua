
-- Executable
-------------------------------------------

project "Haize_Executable"
	local ProjectRoot = os.getcwd()
	
	dependson("Haize_Core")
	language "C++"
	targetname "Haize"
	targetdir (SolutionRoot.."/bin")
	kind "ConsoleApp"

	files	{
		ProjectRoot.."/main/main.cpp"
	}

	links { "Muon_Core", "Haize_Core" }

	filter {}
