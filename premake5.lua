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
IncludeDir["Glad"] = "Amber/vendor/Glad/include"
IncludeDir["GLFW"] = "Amber/vendor/GLFW/include"
IncludeDir["glm"] = "Amber/vendor/glm"
IncludeDir["ImGui"] = "Amber/vendor/ImGui"
IncludeDir["stb"] = "Amber/vendor/stb/include"

group "Dependencies"
    include "Amber/vendor/GLFW"
    include "Amber/vendor/Glad"
    include "Amber/vendor/ImGui"

group ""

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
        "%{prj.name}/src/**.cpp"
    }
    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor",
        "%{IncludeDir.Assimp}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.stb}"
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

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        defines "AB_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "AB_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "AB_DIST"
        runtime "Release"
        optimize "on"

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
        "%{prj.name}/src/**.cpp"
    }
    includedirs
    {
        "%{prj.name}/src",
        "Amber/src",
        "Amber/vendor",
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
        "%{prj.name}/src/**.cpp"
    }
    includedirs
    {
        "%{prj.name}/src",
        "Amber/src",
        "Amber/vendor",
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