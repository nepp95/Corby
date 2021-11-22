include "./vendor/premake/premake_customization/solution_items.lua"

workspace "Engine"
	architecture "x86_64"
	startproject "Editor"
	
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	solution_items
	{
		".editorconfig"
	}

	flags
	{
		"MultiProcessorCompile"
	}

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
	IncludeDir = {}
	IncludeDir["GLFW"] = "%{wks.location}/Engine/vendor/GLFW/include"
	IncludeDir["Glad"] = "%{wks.location}/Engine/vendor/glad/include"
	IncludeDir["ImGui"] = "%{wks.location}/Engine/vendor/imgui"
	IncludeDir["glm"] = "%{wks.location}/Engine/vendor/glm"
	IncludeDir["stb_image"] = "%{wks.location}/Engine/vendor/stb_image"
	IncludeDir["entt"] = "%{wks.location}/Engine/vendor/entt/include"
	
	group "Dependencies"
		include "vendor/premake"
		include "Engine/vendor/GLFW"
		include "Engine/vendor/glad"
		include "Engine/vendor/imgui"

	group ""

	include "Engine"
	include "Sandbox"
	include "Editor"
