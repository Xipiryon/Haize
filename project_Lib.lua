
-- Library
-------------------------------------------
function flexlemon(ProjectRoot)
	local generatedPath = ProjectRoot.."/src/Haize/Parser/lemon/"
	local lemonFile = "lemon.y"
	local lemonGen = "lemon.c"
	local flexFile = "flex.l"
	local flex = "flex"
	if os.is("windows") then
		flex = "flex.exe"
	end

	os.chdir(generatedPath)
	os.execute(flex.." --nounistd --outfile="..flexFile..".yy.cpp --header-file="..flexFile..".yy.hpp "..flexFile)
	os.execute("lemon -s "..lemonFile)
	os.chdir(ProjectRoot)
end

project "HaizeLibrary"
	local ProjectRoot = os.getcwd()

	language "C++"
	targetname "Haize"
	targetdir (SolutionRoot.."/bin/lib")

	files {
		ProjectRoot.."/src/**.cpp",
		ProjectRoot.."/src/Haize/Parser/lemon/lemon.h",
		ProjectRoot.."/src/Haize/Parser/lemon/lemon.c",
		ProjectRoot.."/src/Haize/Parser/lemon/flex.l.yy.hpp",
		ProjectRoot.."/src/Haize/Parser/lemon/flex.l.yy.cpp",
		ProjectRoot.."/include/**.hpp",
	}
	filter "Debug*"
		links	{ "Muon-d" }
	filter "Release*"
		links { "Muon" }

	filter "*DLL"
		defines { "HAIZE_EXPORTS" }

newaction {
	trigger	 = "flexlemon",
	description = "Generate Lemon files",

	execute = function ()
		flexlemon(ProjectRoot)
	end
}