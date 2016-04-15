
-- Library
-------------------------------------------

project "Haize_Core"
	local ProjectRoot = os.getcwd()

	language "C++"
	targetname "Haize"
	targetdir (SolutionRoot.."/bin/lib")

	if _OPTIONS["buildmuon"] then
		dependson("Muon_Core")
	end

	defines { "HAIZE_EXPORTS" }

	files {
		ProjectRoot.."/src/**.cpp",
		ProjectRoot.."/include/**.hpp",
	}

	links { "Muon_Core" }

	filter "*DLL"
		if os.is("windows") then
			postbuildcommands { string.gsub("copy "..SolutionRoot.."/bin/lib/Haize*.dll "..SolutionRoot.."/bin/", "/", "\\") }
		end

	filter {}
