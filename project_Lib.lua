
-- Library
-------------------------------------------

project "Haize"
	local ProjectRoot = os.getcwd()

	language "C++"
	targetname "Haize"
	targetdir (SolutionRoot.."/bin/lib")

	if _OPTIONS["buildmuon"] then 
		dependson("Muon")
	end

	if os.is("windows") then
		postbuildcommands { string.gsub("copy "..SolutionRoot.."/bin/lib/Haize*.dll "..SolutionRoot.."/bin/", "/", "\\") }
	else
		postbuildcommands { "cp "..SolutionRoot.."/bin/lib/Haize*.so "..SolutionRoot.."/bin/" }
	end

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
