
project "Haize"
	language "C++"
	targetdir (SolutionRoot.."/bin/lib")

	if os.is("windows") then
		-- Because on Windows, you can't start a program if .dll are not in the same folder...
		postbuildcommands { string.gsub("copy "..ProjectRoot.."/bin/lib/*.dll "..ProjectRoot.."/bin/", "/", "\\") }
	end

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

