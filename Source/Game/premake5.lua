include "../../Premake/extensions.lua"

workspace "MiggEngine"
	location "../../"
	startproject "Game"
	architecture "x64"

	configurations {
		"Debug",
		"Release"
	}

-- include for common stuff 
include "../../Premake/common.lua"

include (dirs.include)
include (dirs.engine)


-------------------------------------------------------------
project "Game"
	location (dirs.projectfiles)
	dependson { "Include", "Engine" }
		
	-- kind "ConsoleApp"
	kind "WindowedApp"
	language "C++"
	cppdialect "C++17"

	debugdir "%{dirs.bin}"
	targetdir ("%{dirs.bin}")
	targetname("%{prj.name}_%{cfg.buildcfg}")
	objdir ("%{dirs.temp}/%{prj.name}/%{cfg.buildcfg}")

	links {"Include", "Engine"}

	includedirs { 
	".",
	dirs.include, 
	dirs.include .. "imgui/",
	dirs.include .. "DirectXTK/",
	dirs.include .. "nlohmann/",
	dirs.include .. "assimp/",
	dirs.engine, 
	dirs.extra_engine_dirs,
	dirs.extra_game_dirs
	}

	files {
		"**.h",
		"**.cpp",
	}

	libdirs { dirs.lib, dirs.dependencies,dirs.lib_release,dirs.lib_debug }

	verify_or_create_settings("Game")
	
	filter "configurations:Debug"
		defines {"_DEBUG"}
		runtime "Debug"
		symbols "on"
		files {"tools/**"}
		includedirs {"tools/"}
		links {
			"DirectXTKd.lib",
			"assimp-vc143-mtd.lib",
		}
	filter "configurations:Release"
		defines "_RELEASE"
		runtime "Release"
		optimize "on"
		files {"tools/**"}
		includedirs {"tools/"}
		links {
			"DirectXTK.lib",
			"assimp-vc143-mt.lib",
		}
	filter "system:windows"
--		kind "StaticLib"
		staticruntime "off"
		symbols "On"		
		systemversion "latest"
		warnings "Extra"
		--conformanceMode "On"
		--buildoptions { "/permissive" }
		flags { 
		--	"FatalWarnings", -- would be both compile and lib, the original didn't set lib
			"FatalCompileWarnings",
			"MultiProcessorCompile"
		}
		
		defines {
			"WIN32",
			"_LIB", 
		}