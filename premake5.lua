include "./vendor/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"

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
	
	group "Dependencies"
		include "vendor/premake"
		include "Engine/vendor/GLFW"
		include "Engine/vendor/glad"
		include "Engine/vendor/imgui"
		include "Engine/vendor/yaml-cpp"
	group ""

	include "Engine"
	include "Sandbox"
	include "Editor"
