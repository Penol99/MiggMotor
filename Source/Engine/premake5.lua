include "../../Premake/common.lua"


include (dirs.include)
include (dirs.game)

project "Engine"
	location (dirs.projectfiles)

	language "C++"
	cppdialect "C++17"

	targetdir (dirs.lib)
	targetname("%{prj.name}_%{cfg.buildcfg}")
	objdir ("%{dirs.temp}/%{prj.name}/%{cfg.buildcfg}")

	files {
		"**.h",
		"**.cpp",
		"**.hlsl",
		"**.hlsli",
	}

	includedirs {
		".",
		dirs.include .. "/**",
		dirs.engine .. "/**",
	}

	filter "configurations:Debug"
		defines {"_DEBUG"}
		runtime "Debug"
		symbols "on"
	filter "configurations:Release"
		defines "_RELEASE"
		runtime "Release"
		optimize "on"
	filter "system:windows"
		kind "StaticLib"
		staticruntime "off"
		symbols "On"		
		systemversion "latest"
		warnings "Extra"
		sdlchecks "true"
		--conformanceMode "On"
		--buildoptions { "/permissive" }
		flags { 
		--	"FatalWarnings", -- would be both compile and lib, the original didn't set lib
			"FatalCompileWarnings",
			"MultiProcessorCompile"
		}
		links {
			"DXGI"
		}

		defines {
			"WIN32",
			"_CRT_SECURE_NO_WARNINGS", 
			"_LIB", 
			"_WIN32_WINNT=0x0601",
			'JSON_EXPORT_PATH="' .. jsonDir.engine:gsub("%\\", "/") .. '/"',
		}

	shadermodel("5.0")
	local shader_dir = dirs.shaders.absolute
	os.mkdir(shader_dir)

	filter("files:**.hlsl")
		flags("ExcludeFromBuild")
		shaderobjectfileoutput(shader_dir.."%{file.basename}"..".cso")

	filter("files:**Pixel.hlsl")
		removeflags("ExcludeFromBuild")
		shadertype("Pixel")

	filter("files:**Vertex.hlsl")
		removeflags("ExcludeFromBuild")
		shadertype("Vertex")
		
	filter("files:**Compute.hlsl")
		removeflags("ExcludeFromBuild")
		shadertype("Compute")

	filter("files:**Geometry.hlsl")
		removeflags("ExcludeFromBuild")
		shadertype("Geometry")

	-- Warnings as errors
	shaderoptions({"/WX"})
