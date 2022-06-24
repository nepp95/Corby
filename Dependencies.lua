
-- Corby Dependencies

VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["stb_image"] = "%{wks.location}/Corby/vendor/stb_image"
IncludeDir["yaml_cpp"] = "%{wks.location}/Corby/vendor/yaml-cpp/include"
IncludeDir["Box2D"] = "%{wks.location}/Corby/vendor/box2d/include"
IncludeDir["GLFW"] = "%{wks.location}/Corby/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Corby/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/Corby/vendor/ImGui"
IncludeDir["ImGuizmo"] = "%{wks.location}/Corby/vendor/ImGuizmo"
IncludeDir["glm"] = "%{wks.location}/Corby/vendor/glm"
IncludeDir["entt"] = "%{wks.location}/Corby/vendor/entt/include"
IncludeDir["shaderc"] = "%{wks.location}/Corby/vendor/shaderc"
IncludeDir["SPIRV_Cross"] = "%{wks.location}/Corby/vendor/spirv_cross"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

LibraryDir = {}

LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"

Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"
