
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

	defines { "HAIZE_EXPORTS" }

	filter "*DLL"
		if os.is("windows") then
			postbuildcommands { string.gsub("copy "..SolutionRoot.."/bin/lib/Haize*.dll "..SolutionRoot.."/bin/", "/", "\\") }
		else
			postbuildcommands { "find "..SolutionRoot.."/bin/lib/ -name libHaize*.so -exec cp {} "..SolutionRoot.."/bin/ \\;" }
		end

	files {
		ProjectRoot.."/src/**.cpp",
		ProjectRoot.."/include/**.hpp",
	}

	filter "Debug*"
		links { "Muon-d" }

	filter "Release*"
		links { "Muon-r" }

	filter "Release*"
		links { "Muon-f" }

	filter {}
