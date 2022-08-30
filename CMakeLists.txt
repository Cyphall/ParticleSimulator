cmake_minimum_required(VERSION 3.20)

set(CMAKE_TOOLCHAIN_FILE "vcpkg/scripts/buildsystems/vcpkg.cmake")
set(VCPKG_TARGET_TRIPLET "x64-windows-static")

cmake_policy(SET CMP0091 NEW)

project(ParticleSimulator)

add_executable(ParticleSimulator src/ParticleSimulator/Main.cpp src/ParticleSimulator/VKObject/VKContext.cpp src/ParticleSimulator/VKObject/VKContext.h src/ParticleSimulator/Helper/FileHelper.cpp src/ParticleSimulator/Helper/FileHelper.h src/ParticleSimulator/VKObject/VKBuffer.h src/ParticleSimulator/LibImpl.cpp src/ParticleSimulator/Rendering/Renderer.cpp src/ParticleSimulator/Rendering/Renderer.h src/ParticleSimulator/Rendering/RenderPass/ParticleViewPass.cpp src/ParticleSimulator/Rendering/RenderPass/ParticleViewPass.h src/ParticleSimulator/VKObject/VKImageBase.cpp src/ParticleSimulator/VKObject/VKImageBase.h src/ParticleSimulator/VKObject/VKSwapchainImage.cpp src/ParticleSimulator/VKObject/VKSwapchainImage.h src/ParticleSimulator/VKObject/VKShader.cpp src/ParticleSimulator/VKObject/VKShader.h src/ParticleSimulator/VKObject/VKSwapchain.cpp src/ParticleSimulator/VKObject/VKSwapchain.h src/ParticleSimulator/VKObject/Dynamic/VKDynamicCommandBuffer.cpp src/ParticleSimulator/VKObject/Dynamic/VKDynamicCommandBuffer.h src/ParticleSimulator/VKObject/Dynamic/VKDynamicBuffer.h src/ParticleSimulator/RenderContext.h src/ParticleSimulator/RenderContext.cpp src/ParticleSimulator/Camera.cpp src/ParticleSimulator/Camera.h src/ParticleSimulator/Helper/MathHelper.cpp src/ParticleSimulator/Helper/MathHelper.h src/ParticleSimulator/VKObject/Pipeline/VKGraphicsPipeline.cpp src/ParticleSimulator/VKObject/Pipeline/VKGraphicsPipeline.h src/ParticleSimulator/VKObject/Pipeline/VKPipelineVertexInputLayoutInfo.cpp src/ParticleSimulator/VKObject/Pipeline/VKPipelineVertexInputLayoutInfo.h src/ParticleSimulator/VKObject/Pipeline/VKPipelineViewport.h src/ParticleSimulator/VKObject/VKDescriptorSetLayout.cpp src/ParticleSimulator/VKObject/VKDescriptorSetLayout.h src/ParticleSimulator/VKObject/VKImage.cpp src/ParticleSimulator/VKObject/VKImage.h src/ParticleSimulator/VKObject/Dynamic/VKDynamicSemaphore.cpp src/ParticleSimulator/VKObject/Dynamic/VKDynamicSemaphore.h src/ParticleSimulator/VKObject/VKSwapchainSemaphore.cpp src/ParticleSimulator/VKObject/VKSwapchainSemaphore.h src/ParticleSimulator/VKObject/Dynamic/VKDynamicFence.cpp src/ParticleSimulator/VKObject/Dynamic/VKDynamicFence.h src/ParticleSimulator/VKObject/VKDescriptorSet.cpp src/ParticleSimulator/VKObject/VKDescriptorSet.h src/ParticleSimulator/VKObject/VKDescriptorSetLayoutBinding.h src/ParticleSimulator/VKObject/Dynamic/VKDynamicDescriptorSet.cpp src/ParticleSimulator/VKObject/Dynamic/VKDynamicDescriptorSet.h src/ParticleSimulator/VKObject/Pipeline/VKPipelineLayoutInfo.cpp src/ParticleSimulator/VKObject/Pipeline/VKPipelineLayoutInfo.h src/ParticleSimulator/VKObject/VKDescriptorSetLayoutBuilder.cpp src/ParticleSimulator/VKObject/VKDescriptorSetLayoutBuilder.h src/ParticleSimulator/VKObject/Dynamic/VKDynamicImage.cpp src/ParticleSimulator/VKObject/Dynamic/VKDynamicImage.h src/ParticleSimulator/VKObject/Pipeline/VKPipeline.cpp src/ParticleSimulator/VKObject/Pipeline/VKPipeline.h src/ParticleSimulator/VKObject/Pipeline/VKComputePipeline.cpp src/ParticleSimulator/VKObject/Pipeline/VKComputePipeline.h src/ParticleSimulator/Rendering/RenderPass/ComputePass.cpp src/ParticleSimulator/Rendering/RenderPass/ComputePass.h src/ParticleSimulator/Rendering/ParticleStructs.h src/ParticleSimulator/Rendering/RenderPass/GravityViewPass.cpp src/ParticleSimulator/Rendering/RenderPass/GravityViewPass.h)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC") # MSVC
	set_property(TARGET ParticleSimulator PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC") # clang-cl
	set_property(TARGET ParticleSimulator PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	target_compile_options(ParticleSimulator PRIVATE /EHsc)
endif()

target_compile_features(ParticleSimulator PRIVATE cxx_std_20)

target_compile_definitions(ParticleSimulator PRIVATE GLM_FORCE_DEPTH_ZERO_TO_ONE)
target_compile_definitions(ParticleSimulator PRIVATE GLFW_INCLUDE_NONE)
target_compile_definitions(ParticleSimulator PRIVATE VK_NO_PROTOTYPES)
target_compile_definitions(ParticleSimulator PRIVATE VULKAN_HPP_NO_SETTERS)
target_compile_definitions(ParticleSimulator PRIVATE VULKAN_HPP_NO_NODISCARD_WARNINGS)
target_compile_definitions(ParticleSimulator PRIVATE VULKAN_HPP_NO_SMART_HANDLE)

find_package(Vulkan REQUIRED)
target_link_libraries(ParticleSimulator PRIVATE Vulkan::Headers)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(ParticleSimulator PRIVATE glfw)

find_package(glm CONFIG REQUIRED)
target_link_libraries(ParticleSimulator PRIVATE glm::glm)

target_include_directories(ParticleSimulator PRIVATE "src")
target_include_directories(ParticleSimulator SYSTEM PRIVATE "vendor/VulkanMemoryAllocator-Hpp/include")

add_custom_target(copy_resources
		COMMAND python "${PROJECT_SOURCE_DIR}/copy_resources.py" "${PROJECT_SOURCE_DIR}/resources" "${PROJECT_BINARY_DIR}/resources" "${Vulkan_GLSLC_EXECUTABLE}")
add_dependencies(ParticleSimulator copy_resources)