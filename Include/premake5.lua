include "../../Premake/common.lua"
project "Include"
	location (dirs.projectfiles)
		
	language "C++"
	cppdialect "C++17"

	targetdir (dirs.lib)
	targetname("%{prj.name}_%{cfg.buildcfg}")
	objdir ("%{dirs.temp}/%{prj.name}/%{cfg.buildcfg}")

	--pchheader "stdafx.h"
	--pchsource "Source/%{prj.name}/stdafx.cpp"

	files {
		"**.h",
		"**.hpp",
		"**.inl",
		"**.c",
		"**.cpp",
	}

	excludes {
		"ffmpeg-2.0/**.h",
		"ffmpeg-2.0/**.c",
		"ffmpeg-2.0/**.cpp",
	}

	includedirs {
		".",
		"source/",
		"directxtk/",
		"nlohmann/",
		"imgui/",
		"imnodes/",
		"XAudio2/",
	}

	libdirs { "Lib/" }

	defines {"_CONSOLE"}

	filter "configurations:Debug"
		defines {"_DEBUG"}
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines {"_RELEASE"}
		runtime "Release"
		optimize "on"


	filter "system:windows"
		kind "StaticLib"
		staticruntime "off"
		symbols "On"		
		systemversion "latest"
		--warnings "Extra"
		--conformanceMode "On"
		--buildoptions { "/permissive" }
		flags { 
		--	"FatalWarnings", -- would be both compile and lib, the original didn't set lib
		--	"FatalCompileWarnings",
			"MultiProcessorCompile"
		}
		links {

		}

		defines { "_WIN32_WINNT=0x0600" }