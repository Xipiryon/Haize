
-- Library
-------------------------------------------
function genparser(ProjectRoot)
	local generatedPath = ProjectRoot.."/src/Haize/Parser/generated/"
	local flex = "flex"
	local yacc = "bison"
	if os.is("windows") then
		flex = "flex.exe"
		yacc = "bison.exe"
	end

	os.chdir(generatedPath)
	os.execute(flex.." --nounistd --outfile=flex.l.cpp --header-file=flex.l.hpp flex.l")
	os.execute(yacc.." -v -d -o yacc.cpp yacc.y")
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
		-- Flex/Bison and related files
		ProjectRoot.."/src/Haize/Parser/**.hpp",
		ProjectRoot.."/src/Haize/Parser/generated/yacc.y",
		ProjectRoot.."/src/Haize/Parser/generated/flex.l",
	}
	filter "Debug*"
		links { "Muon-d" }
	filter "Release*"
		links { "Muon" }

	filter "*DLL"
		defines { "HAIZE_EXPORTS" }

newaction {
	trigger	 = "genparser",
	description = "Generate Flex/Bison files",

	execute = function ()
		genparser(ProjectRoot)
	end
}