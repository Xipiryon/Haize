------------------------------
-- Global Variable
------------------------------

-- Prefix Install specify where it should look for custom
-- project header & library files
if G_Install == nil then

	G_Install = {}
	-- Default Value on Windows & Unix Platform
	if os.is("windows") then
		G_Install.Root = "C:/Premake/"
	else
		G_Install.Root = "/usr/local/"
	end
end
if not G_Install.Header then G_Install.Header = G_Install.Root.."include" end
if not G_Install.Lib then G_Install.Lib = G_Install.Root.."lib" end

HaizeRoot = os.getcwd()

-- Generate Lex/Yacc files
function generateParserFiles(executeOsCommand)
	local parserPath = HaizeRoot.."/src/Haize/Parser/YACC/"
	if os.is("windows") then
		lex = "flex.exe"
		yacc = "bison.exe"
	else -- Unix
		lex = "flex"
		yacc = "bison"
	end
	local generatedPath = parserPath.."generated/"
	if(not os.isdir(generatedPath)) then
		if os.mkdir(generatedPath) then print("Creating "..generatedPath) end
	end
	
	os.execute(lex.." -o "..generatedPath.."flex.haize.cpp "..parserPath.."flex.haize.ll")
	os.execute(yacc.." -v -d -o "..generatedPath.."yacc.haize.cpp "..parserPath.."yacc.haize.yy")
end

------------------------------
-- Solution
------------------------------

solution "Haize"
	startproject "HaizeExecutable"
	configurations { "DebugDLL", "DebugLib", "ReleaseLib", "ReleaseDLL" }

	if not os.is("windows") then
		buildoptions { "--std=c++11" }
		linkoptions { "-Wl,-rpath,"..HaizeRoot.."/bin/lib/" }
	end

	includedirs {
		HaizeRoot.."/include",
		G_Install.Header,
	}

	libdirs {
		HaizeRoot.."/bin/lib",
		G_Install.Lib
	}

	filter "Debug*"
		targetsuffix "-d"
		flags	{ "Symbols" }

	filter "Release*"
		optimize "Speed"
		flags	{ "LinkTimeOptimization", "NoRTTI" }

	filter  "*Lib"
		kind "StaticLib"

	filter  "*DLL"
		kind "SharedLib"
		
	filter "Debug*"
		defines { "MUON_DEBUG"}
		
	filter "Release*"
		defines { "MUON_RELEASE"}

------------------------------
-- Project
------------------------------

-- Library
-------------------------------------------

project "Haize"
	language "C++"
	targetdir(HaizeRoot.."/bin/lib")
	
	if os.is("windows") then
		postbuildcommands { string.gsub("copy "..HaizeRoot.."/bin/lib/*.dll "..HaizeRoot.."/bin/", "/", "\\") }
	end

	files {
		HaizeRoot.."/src/**.cpp",
		HaizeRoot.."/include/**.hpp",
		-- Flex/Bison and related files
		HaizeRoot.."/src/Haize/Parser/**.hpp",
		HaizeRoot.."/src/Haize/Parser/YACC/yacc.haize.yy",
		HaizeRoot.."/src/Haize/Parser/YACC/flex.haize.ll",
	}
	filter "Debug*"
		links	{ "Muon-d" }
	filter "Release*"
		links { "Muon" }
		
	filter "*DLL"
		defines { "HAIZE_EXPORTS" }


-- Console Application 
-------------------------------------------

project "HaizeExecutable"
	language "C++"
	targetname "Haize"
	targetdir "bin"
	kind "ConsoleApp"

	files	{
		HaizeRoot.."/main/main.cpp"
	}

	filter "Debug*"
		links	{ "Haize-d", "Muon-d" }
		
	filter "Release*"
		links { "Haize", "Muon" }

------------------------------
-- Actions
------------------------------

newaction {
	trigger = "genparser",
	description = "Generate Flex & Bison files",
	execute = function() 
		generateParserFiles();
	end
}

newaction {
	trigger	 = "install",
	description = "Install developpment files & library",
	execute = function ()
		print("** Installing Header files in: "..G_Install.Header.." **")

		local incDir = HaizeRoot.."/include/"
		local libDir = HaizeRoot.."/bin/lib/"

		-- Create required folders
		local dirList = os.matchdirs(incDir.."**")
		for _,fdir in pairs(dirList) do
			local destDir = G_Install.Header..string.sub(fdir, #incDir)

			if(not os.isdir(destDir)) then
				if os.mkdir(destDir) then print("Creating "..destDir) end
			end
		end

		-- Copy files
		local fileList = os.matchfiles(incDir.."**")
		for _,fpath in pairs(fileList) do
			local destFile = G_Install.Header..string.sub(fpath, #incDir)
			if os.copyfile(fpath, destFile) then print("Installing "..destFile) end
		end


		-- LIBRARY
		print("** Installing Library files in: "..G_Install.Lib.." **")
		local destDir = G_Install.Lib

		-- Create required folders
		if(not os.isdir(destDir)) then
			if os.mkdir(destDir) then print("Creating "..destDir) end
		end

		local exts = {}
		if os.is("windows") then
			exts[0] = ".dll"
			exts[1] = ".lib"
		else
			exts[0] = ".so"
		end

		-- Copy files
		for _,ext in pairs(exts) do
			local fileList = os.matchfiles(libDir.."**"..ext)
			for _,fpath in pairs(fileList) do
				local destFile = G_Install.Lib..string.sub(fpath, #libDir)
				if os.copyfile(fpath, destFile) then print("Installing "..destFile) end
			end
		end
	end
}