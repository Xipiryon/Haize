
-- Library
-------------------------------------------
function flexlemon(ProjectRoot)
	local generatedPath = ProjectRoot.."/src/Haize/Parser/lemon/"
	local lemonFile = "lemon.haize"
	local lemonGen = "lemon.c"
	local flexFile = "flex.haize"
	local flex = "flex"
	if os.is("windows") then
		flex = "flex.exe"
	end

	os.chdir(generatedPath)
	os.execute(flex.." --outfile="..flexFile..".yy.cpp --header-file="..flexFile..".yy.hpp "..flexFile)
	os.execute("rm "..lemonGen.."pp") -- remove .cpp
	os.execute("lemon -s "..lemonFile)
	os.execute("mv "..lemonGen.." "..lemonGen.."pp") -- rename .c to .cpp
	os.chdir(ProjectRoot)
end

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