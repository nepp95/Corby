include "./vendor/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"

workspace "Corby"
	architecture "x86_64"
	startproject "CorbyEd"
	
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
		include "Corby/vendor/box2d"
		include "Corby/vendor/GLFW"
		include "Corby/vendor/glad"
		include "Corby/vendor/imgui"
		include "Corby/vendor/yaml-cpp"
	group ""

	include "Corby"
	include "Sandbox"
	include "CorbyEd"
