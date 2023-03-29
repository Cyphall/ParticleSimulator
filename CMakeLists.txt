cmake_minimum_required(VERSION 3.20)

set(CMAKE_TOOLCHAIN_FILE "vcpkg/scripts/buildsystems/vcpkg.cmake")
set(VCPKG_TARGET_TRIPLET "x64-windows-static")

cmake_policy(SET CMP0091 NEW)

project(ParticleSimulator)

add_executable(ParticleSimulator src/ParticleSimulator/Main.cpp src/ParticleSimulator/Helper/FileHelper.cpp src/ParticleSimulator/Helper/FileHelper.h src/ParticleSimulator/LibImpl.cpp src/ParticleSimulator/Rendering/Renderer.cpp src/ParticleSimulator/Rendering/Renderer.h src/ParticleSimulator/Rendering/RenderPass/ParticleViewPass.cpp src/ParticleSimulator/Rendering/RenderPass/ParticleViewPass.h src/ParticleSimulator/RenderContext.h src/ParticleSimulator/RenderContext.cpp src/ParticleSimulator/Camera.cpp src/ParticleSimulator/Camera.h src/ParticleSimulator/Helper/MathHelper.cpp src/ParticleSimulator/Helper/MathHelper.h src/ParticleSimulator/Rendering/RenderPass/ComputePass.cpp src/ParticleSimulator/Rendering/RenderPass/ComputePass.h src/ParticleSimulator/Rendering/ParticleStructs.h src/ParticleSimulator/Rendering/RenderPass/GravityViewPass.cpp src/ParticleSimulator/Rendering/RenderPass/GravityViewPass.h src/ParticleSimulator/VKObject/VKContext.cpp src/ParticleSimulator/VKObject/VKContext.h src/ParticleSimulator/VKObject/VKDynamic.h src/ParticleSimulator/VKObject/VKObject.cpp src/ParticleSimulator/VKObject/VKObject.h src/ParticleSimulator/VKObject/VKPtr.h src/ParticleSimulator/VKObject/VKSwapchain.cpp src/ParticleSimulator/VKObject/VKSwapchain.h src/ParticleSimulator/VKObject/Buffer/VKBuffer.h src/ParticleSimulator/VKObject/Buffer/VKBufferBase.h src/ParticleSimulator/VKObject/CommandBuffer/VKCommandBuffer.cpp src/ParticleSimulator/VKObject/CommandBuffer/VKCommandBuffer.h src/ParticleSimulator/VKObject/DescriptorSet/VKDescriptorSet.cpp src/ParticleSimulator/VKObject/DescriptorSet/VKDescriptorSet.h src/ParticleSimulator/VKObject/DescriptorSet/VKDescriptorSetLayout.cpp src/ParticleSimulator/VKObject/DescriptorSet/VKDescriptorSetLayout.h src/ParticleSimulator/VKObject/DescriptorSet/VKDescriptorSetLayoutBinding.h src/ParticleSimulator/VKObject/DescriptorSet/VKDescriptorSetLayoutInfo.cpp src/ParticleSimulator/VKObject/DescriptorSet/VKDescriptorSetLayoutInfo.h src/ParticleSimulator/VKObject/Fence/VKFence.cpp src/ParticleSimulator/VKObject/Fence/VKFence.h src/ParticleSimulator/VKObject/Image/VKImage.cpp src/ParticleSimulator/VKObject/Image/VKImage.h src/ParticleSimulator/VKObject/Image/VKImageBase.cpp src/ParticleSimulator/VKObject/Image/VKImageBase.h src/ParticleSimulator/VKObject/Image/VKSwapchainImage.cpp src/ParticleSimulator/VKObject/Image/VKSwapchainImage.h src/ParticleSimulator/VKObject/Pipeline/VKComputePipeline.cpp src/ParticleSimulator/VKObject/Pipeline/VKComputePipeline.h src/ParticleSimulator/VKObject/Pipeline/VKGraphicsPipeline.cpp src/ParticleSimulator/VKObject/Pipeline/VKGraphicsPipeline.h src/ParticleSimulator/VKObject/Pipeline/VKPipeline.cpp src/ParticleSimulator/VKObject/Pipeline/VKPipeline.h src/ParticleSimulator/VKObject/Pipeline/VKPipelineAttachmentInfo.cpp src/ParticleSimulator/VKObject/Pipeline/VKPipelineAttachmentInfo.h src/ParticleSimulator/VKObject/Pipeline/VKPipelineLayout.cpp src/ParticleSimulator/VKObject/Pipeline/VKPipelineLayout.h src/ParticleSimulator/VKObject/Pipeline/VKPipelineLayoutInfo.cpp src/ParticleSimulator/VKObject/Pipeline/VKPipelineLayoutInfo.h src/ParticleSimulator/VKObject/Pipeline/VKPipelineVertexInputLayoutInfo.cpp src/ParticleSimulator/VKObject/Pipeline/VKPipelineVertexInputLayoutInfo.h src/ParticleSimulator/VKObject/Pipeline/VKPipelineViewport.h src/ParticleSimulator/VKObject/Queue/VKQueue.cpp src/ParticleSimulator/VKObject/Queue/VKQueue.h src/ParticleSimulator/VKObject/Sampler/VKSampler.cpp src/ParticleSimulator/VKObject/Sampler/VKSampler.h src/ParticleSimulator/VKObject/Semaphore/VKSemaphore.cpp src/ParticleSimulator/VKObject/Semaphore/VKSemaphore.h src/ParticleSimulator/VKObject/Shader/VKShader.cpp src/ParticleSimulator/VKObject/Shader/VKShader.h)

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

target_include_directories(ParticleSimulator PRIVATE "src")

find_package(Vulkan REQUIRED)
target_link_libraries(ParticleSimulator PRIVATE Vulkan::Headers)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(ParticleSimulator PRIVATE glfw)

find_package(glm CONFIG REQUIRED)
target_link_libraries(ParticleSimulator PRIVATE glm::glm)

add_subdirectory("vendor/VulkanMemoryAllocator/66afe099f1cf1f79c270471e9c0f02139072057d")
target_link_libraries(ParticleSimulator PRIVATE VulkanMemoryAllocator)

add_subdirectory("vendor/VulkanMemoryAllocator-Hpp/f915eb0ef170560661891afaedff4b93ef05d723")
target_link_libraries(ParticleSimulator PRIVATE VulkanMemoryAllocator-Hpp)

add_custom_target(copy_resources
		COMMAND python "${PROJECT_SOURCE_DIR}/copy_resources.py" "${PROJECT_SOURCE_DIR}/resources" "${PROJECT_BINARY_DIR}/resources" "${Vulkan_GLSLC_EXECUTABLE}")
add_dependencies(ParticleSimulator copy_resources)