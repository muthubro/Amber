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
    startproject "Sandbox"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["Glad"] = "Amber/vendor/Glad/include"
IncludeDir["GLFW"] = "Amber/vendor/GLFW/include"
IncludeDir["glm"] = "Amber/vendor/glm"
IncludeDir["ImGui"] = "Amber/vendor/ImGui"
IncludeDir["spdlog"] = "Amber/vendor/spdlog/include"
IncludeDir["stb_image"] = "Amber/vendor/stb_image"

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
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/vendor/glm/glm/**.hpp",
        "%{prj.name}/vendor/glm/glm/**.inl",
        "%{prj.name}/vendor/stb_image/**.h",
        "%{prj.name}/vendor/stb_image/**.cpp"
    }
    includedirs
    {
        "%{prj.name}/src",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.spdlog}"
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
        "Amber/src",
        "Amber/vendor",
        "%{IncludeDir.glm}",
        "%{IncludeDir.spdlog}"
    }
    links
    {
        "Amber"
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