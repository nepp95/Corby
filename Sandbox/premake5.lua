project "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"%{wks.location}/Engine/vendor/spdlog/include",
		"%{wks.location}/Engine/src",
		"%{wks.location}/Engine/vendor",
		"%{IncludeDir.glm}",
		"%{IncludeDir.entt}"
	}

	links
	{
		"Engine"
	}

	filter "system:windows"
		systemversion "latest"

	filter "system:macosx"
		systemversion "latest"

	filter "configurations:Debug"
		defines "ENG_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "ENG_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "ENG_DIST"
		runtime "Release"
		optimize "on"
