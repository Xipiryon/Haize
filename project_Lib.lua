
-- Library
-------------------------------------------

project "HaizeLibrary"
	local ProjectRoot = os.getcwd()

	language "C++"
	targetname "Haize"
	targetdir (SolutionRoot.."/bin/lib")

	files {
		ProjectRoot.."/src/**.cpp",
		ProjectRoot.."/include/**.hpp",
	}
	filter "Debug*"
		links { "Muon-d" }
	filter "Release*"
		links { "Muon" }

	filter "*DLL"
		defines { "HAIZE_EXPORTS" }
