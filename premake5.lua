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

------------------------------
-- Solution
------------------------------

solution "Haize"
	startproject "HaizeExecutable"
	configurations { "DebugDLL", "DebugLib", "ReleaseLib", "ReleaseDLL" }

	if os.is("windows") then
		implibdir "lib"
	else
		buildoptions { "--std=c++11" }
		linkoptions { "-Wl,-rpath,lib" }
	end

	-- If option exists, then override G_Install
	if _OPTIONS["basedir"] then
		G_Install.Root = _OPTIONS["basedir"]
		G_Install.Header = _OPTIONS["basedir"].."/include"
		G_Install.Lib = _OPTIONS["basedir"].."/lib"
		print("Install directory has been overwritten to '"..G_Install.Root.."'")
	end

	includedirs {
		"include",
		G_Install.Header,
	}

	libdirs {
		"lib",
		G_Install.Lib
	}

	filter "Debug*"
		targetsuffix "-d"
		optimize "Debug"
		flags	{ "Symbols" }

	filter "Release*"
		optimize "Speed"
		flags	{ "LinkTimeOptimization" }

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
		postbuildcommands { string.gsub("copy lib/*.dll bin/", "/", "\\") }
	end

	files {
		HaizeRoot.."/src/**.cpp",
		HaizeRoot.."/include/**.hpp",
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

-- Unit Tests
-------------------------------------------

if _OPTIONS["unittests"] then

	project "UnitTests"
		language "C++"
		targetname "UnitTests"
		targetdir "bin"
		kind "ConsoleApp"

		files	{
			HaizeRoot.."/unittests/main.cpp"
		}

		filter "Debug*"
			links	{ "Haize-d", "Muon-d" }

		filter "Release*"
			links { "Haize", "Muon" }

end

------------------------------
-- Options
------------------------------

newoption {
	trigger     = "basedir",
	value       = "PATH",
	description = "Folder to search lib & include; default: '"..G_Install.Root.."'",
}

newoption {
	trigger     = "unittests",
	description = "Enable compilation of unit tests",
}

------------------------------
-- Actions
------------------------------

newaction {
	trigger	 = "install",
	description = "Install developpment files & library",
	execute = function ()
		print("** Installing Header files in: "..G_Install.Header.." **")

		local incDir = HaizeRoot.."/include/"
		local libDir = HaizeRoot.."/lib/"

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
			exts[1] = ".a"
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

if os.is("windows") then
	newaction {
		trigger	 = "getlib",
		description = "Retrieve libraries from 'basedir' and put them in bin/",
		execute = function ()
			print("** Retrieving files from: "..G_Install.Lib.." **")

			local libDir = G_Install.Lib

			local destDir = "bin"

			-- Create required folders
			if(not os.isdir(destDir)) then
				if os.mkdir(destDir) then print("Creating "..destDir) end
			end

			-- Copy files
			local fileList = os.matchfiles(libDir.."**Muon**dll")
			for _,fpath in pairs(fileList) do
				local destFile = destDir..string.sub(fpath, 1+#G_Install.Lib)
				if os.copyfile(fpath, destFile) then print("Copying "..fpath.." to "..destDir) end
			end
		end
	}
end
