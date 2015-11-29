
-- Library
-------------------------------------------

project "Haize"
	local ProjectRoot = os.getcwd()

	language "C++"
	targetdir (SolutionRoot.."/bin/lib")

	files {
		ProjectRoot.."/src/**.cpp",
		ProjectRoot.."/include/**.hpp",
		-- Flex/Bison and related files
		ProjectRoot.."/src/Haize/Parser/**.hpp",
		ProjectRoot.."/src/Haize/Parser/YACC/yacc.haize.yy",
		ProjectRoot.."/src/Haize/Parser/YACC/flex.haize.ll",
	}
	filter "Debug*"
		links	{ "Muon-d" }
	filter "Release*"
		links { "Muon" }

	filter "*DLL"
		defines { "HAIZE_EXPORTS" }

