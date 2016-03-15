
-- Library
-------------------------------------------
function lemon(ProjectRoot)
	local lemonPath = ProjectRoot.."/src/Haize/Parser/lemon/"
	local lemonFile = "lemon.haize"
	local lemonGen = "lemon.c"
	os.chdir(lemonPath)
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
	trigger	 = "lemon",
	description = "Generate Lemon files",

	execute = function ()
		lemon(ProjectRoot)
	end
}