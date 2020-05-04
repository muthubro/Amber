project "Amber"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "abpch.h"
	pchsource "src/abpch.cpp"

	IncludeDir = {}
	IncludeDir["Glad"] = "vendor/Glad/include"
	IncludeDir["GLFW"] = "vendor/GLFW/include"
	IncludeDir["glm"] = "vendor/glm"
	IncludeDir["ImGui"] = "vendor/ImGui"
	IncludeDir["spdlog"] = "vendor/spdlog/include"
	IncludeDir["stb_image"] = "vendor/stb_image"

	files
	{
		"src/**.h",
		"src/**.cpp",
		"vendor/glm/glm/**.hpp",
		"vendor/glm/glm/**.inl",
		"vendor/stb_image/**.h",
		"vendor/stb_image/**.cpp"
	}
	includedirs
	{
		"src",
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
