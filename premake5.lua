workspace "Amber"
    architecture "x86_64"
    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }
    flags
    {
        "MultiProcessorCompile"
    }
    startproject "Editor"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["Assimp"] = "Amber/vendor/Assimp/include"
IncludeDir["entt"] = "Amber/vendor/entt/include"
IncludeDir["Glad"] = "Amber/vendor/Glad/include"
IncludeDir["GLFW"] = "Amber/vendor/GLFW/include"
IncludeDir["glm"] = "Amber/vendor/glm"
IncludeDir["ImGui"] = "Amber/vendor/ImGui"
IncludeDir["mono"] = "Amber/vendor/mono/include"
IncludeDir["stb"] = "Amber/vendor/stb/include"
IncludeDir["yaml_cpp"] = "Amber/vendor/yaml-cpp/include"

LibraryDir = {}
LibraryDir["mono"] = "vendor/mono/lib"

group "Dependencies"
    include "Amber/vendor/GLFW"
    include "Amber/vendor/Glad"
    include "Amber/vendor/ImGui"

group "Core"
project "Amber"
    location "Amber"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "abpch.h"
    pchsource "%{prj.name}/src/abpch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.c",
        "%{prj.name}/src/**.hpp",
        "%{prj.name}/src/**.cpp",
        "Amber/vendor/yaml-cpp/src/**.h",
        "Amber/vendor/yaml-cpp/src/**.cpp"
    }
    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor",
        "%{IncludeDir.Assimp}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.mono}",
        "%{IncludeDir.stb}",
        "%{IncludeDir.yaml_cpp}"
    }
    links
    {
        "GLFW",
        "Glad",
        "ImGui",
        "opengl32.lib"
    }

    defines
    {
        "_CRT_SECURE_NO_WARNINGS",
        "GLFW_INCLUDE_NONE"
    }

    filter "files:Amber/src/Amber/Vendor/**.cpp or files:Amber/vendor/**.cpp"
        flags "NoPCH"

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        defines "AB_DEBUG"
        runtime "Debug"
        symbols "on"

        links "%{LibraryDir.mono}/Debug/mono-2.0-sgen.lib"

    filter "configurations:Release"
        defines "AB_RELEASE"
        runtime "Release"
        optimize "on"

        links "%{LibraryDir.mono}/Release/mono-2.0-sgen.lib"

    filter "configurations:Dist"
        defines "AB_DIST"
        runtime "Release"
        optimize "on"

        links "%{LibraryDir.mono}/Release/mono-2.0-sgen.lib"

project "ScriptCore"
    location "ScriptCore"
    kind "SharedLib"
    language "C#"

    targetdir ("Editor/assets/scripts")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.cs"
    }

group "Tools"
project "Editor"
    location "Editor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.c",
        "%{prj.name}/src/**.hpp",
        "%{prj.name}/src/**.cpp"
    }
    includedirs
    {
        "%{prj.name}/src",
        "Amber/src",
        "Amber/vendor",
        "%{IncludeDir.entt}",
        "%{IncludeDir.glm}"
    }
    links
    {
        "Amber"
    }

	postbuildcommands 
	{
		'{COPY} "../Editor/assets" "%{cfg.targetdir}/assets"'
	}

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        defines "AB_DEBUG"
        runtime "Debug"
        symbols "on"
        
        links
        {
            "Amber/vendor/Assimp/bin/Debug/assimp-vc142-mtd.lib"
        }

		postbuildcommands 
		{
            '{COPY} "../Amber/vendor/Assimp/bin/Debug/assimp-vc142-mtd.dll" "%{cfg.targetdir}"',
            '{COPY} "../Amber/vendor/mono/bin/Debug/mono-2.0-sgen.dll" "%{cfg.targetdir}"'
		}

    filter "configurations:Release"
        defines "AB_RELEASE"
        runtime "Release"
        optimize "on"
        
        links
        {
            "Amber/vendor/Assimp/bin/Release/assimp-vc142-mt.lib"
        }

		postbuildcommands 
		{
			'{COPY} "../Amber/vendor/Assimp/bin/Release/assimp-vc142-mt.dll" "%{cfg.targetdir}"',
            '{COPY} "../Amber/vendor/mono/bin/Release/mono-2.0-sgen.dll" "%{cfg.targetdir}"'
		}

    filter "configurations:Dist"
        defines "AB_DIST"
        runtime "Release"
        optimize "on"
        
        links
        {
            "Amber/vendor/Assimp/bin/Release/assimp-vc142-mt.lib"
        }

		postbuildcommands 
		{
			'{COPY} "../Amber/vendor/Assimp/bin/Release/assimp-vc142-mt.dll" "%{cfg.targetdir}"',
            '{COPY} "../Amber/vendor/mono/bin/Release/mono-2.0-sgen.dll" "%{cfg.targetdir}"'
		}

group "Examples"
project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.c",
        "%{prj.name}/src/**.hpp",
        "%{prj.name}/src/**.cpp"
    }
    includedirs
    {
        "%{prj.name}/src",
        "Amber/src",
        "Amber/vendor",
        "%{IncludeDir.entt}",
        "%{IncludeDir.glm}"
    }
    links
    {
        "Amber"
    }

	postbuildcommands 
	{
		'{COPY} "../Sandbox/assets" "%{cfg.targetdir}/assets"'
	}

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        defines "AB_DEBUG"
        runtime "Debug"
        symbols "on"
        
        links
        {
            "Amber/vendor/Assimp/bin/Debug/assimp-vc142-mtd.lib"
        }

		postbuildcommands 
		{
			'{COPY} "../Amber/vendor/Assimp/bin/Debug/assimp-vc142-mtd.dll" "%{cfg.targetdir}"'
		}

    filter "configurations:Release"
        defines "AB_RELEASE"
        runtime "Release"
        optimize "on"
        
        links
        {
            "Amber/vendor/Assimp/bin/Release/assimp-vc142-mt.lib"
        }

		postbuildcommands 
		{
			'{COPY} "../Amber/vendor/Assimp/bin/Release/assimp-vc142-mt.dll" "%{cfg.targetdir}"'
		}

    filter "configurations:Dist"
        defines "AB_DIST"
        runtime "Release"
        optimize "on"
        
        links
        {
            "Amber/vendor/Assimp/bin/Release/assimp-vc142-mt.lib"
        }

		postbuildcommands 
		{
			'{COPY} "../Amber/vendor/Assimp/bin/Release/assimp-vc142-mt.dll" "%{cfg.targetdir}"'
        }
        
project "Terrain"
    location "Examples/Terrain"
    kind "SharedLib"
    language "C#"

    targetdir ("Editor/assets/scripts")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "Examples/%{prj.name}/src/**.cs"
    }

    links
    {
        "ScriptCore"
    }