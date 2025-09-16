cmake_minimum_required(VERSION 3.28)

cmake_path(CONVERT "$ENV{VCPKG_ROOT}" TO_CMAKE_PATH_LIST VCPKG_ROOT)

set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
set(VCPKG_TARGET_TRIPLET "x64-windows-static-md")

include(cmake/Shaders.cmake)

project(ParticleSimulator)

add_executable(ParticleSimulator)

target_sources(ParticleSimulator PRIVATE
	"src/cpp/ParticleSimulator/Camera.cpp"
	"src/cpp/ParticleSimulator/LibImpl.cpp"
	"src/cpp/ParticleSimulator/Main.cpp"
	"src/cpp/ParticleSimulator/RenderContext.cpp"
	"src/cpp/ParticleSimulator/Helper/MathHelper.cpp"
	"src/cpp/ParticleSimulator/Rendering/Renderer.cpp"
	"src/cpp/ParticleSimulator/Rendering/RenderPass/ComputePass.cpp"
	"src/cpp/ParticleSimulator/Rendering/RenderPass/GravityViewPass.cpp"
	"src/cpp/ParticleSimulator/Rendering/RenderPass/ParticleViewPass.cpp"
	"src/cpp/ParticleSimulator/VKObject/VKContext.cpp"
	"src/cpp/ParticleSimulator/VKObject/VKObject.cpp"
	"src/cpp/ParticleSimulator/VKObject/VKSwapchain.cpp"
	"src/cpp/ParticleSimulator/VKObject/CommandBuffer/VKCommandBuffer.cpp"
	"src/cpp/ParticleSimulator/VKObject/DescriptorSet/VKDescriptorSet.cpp"
	"src/cpp/ParticleSimulator/VKObject/DescriptorSet/VKDescriptorSetLayout.cpp"
	"src/cpp/ParticleSimulator/VKObject/DescriptorSet/VKDescriptorSetLayoutInfo.cpp"
	"src/cpp/ParticleSimulator/VKObject/Fence/VKFence.cpp"
	"src/cpp/ParticleSimulator/VKObject/Image/VKImage.cpp"
	"src/cpp/ParticleSimulator/VKObject/Image/VKImageBase.cpp"
	"src/cpp/ParticleSimulator/VKObject/Image/VKSwapchainImage.cpp"
	"src/cpp/ParticleSimulator/VKObject/Pipeline/VKComputePipeline.cpp"
	"src/cpp/ParticleSimulator/VKObject/Pipeline/VKGraphicsPipeline.cpp"
	"src/cpp/ParticleSimulator/VKObject/Pipeline/VKPipeline.cpp"
	"src/cpp/ParticleSimulator/VKObject/Pipeline/VKPipelineAttachmentInfo.cpp"
	"src/cpp/ParticleSimulator/VKObject/Pipeline/VKPipelineLayout.cpp"
	"src/cpp/ParticleSimulator/VKObject/Pipeline/VKPipelineLayoutInfo.cpp"
	"src/cpp/ParticleSimulator/VKObject/Pipeline/VKPipelineVertexInputLayoutInfo.cpp"
	"src/cpp/ParticleSimulator/VKObject/Queue/VKQueue.cpp"
	"src/cpp/ParticleSimulator/VKObject/Sampler/VKSampler.cpp"
	"src/cpp/ParticleSimulator/VKObject/Semaphore/VKSemaphore.cpp"
	"src/cpp/ParticleSimulator/VKObject/Shader/VKShader.cpp"
)

target_sources(ParticleSimulator PRIVATE FILE_SET HEADERS BASE_DIRS "src/cpp" FILES
	"src/cpp/ParticleSimulator/Camera.h"
	"src/cpp/ParticleSimulator/RenderContext.h"
	"src/cpp/ParticleSimulator/Helper/MathHelper.h"
	"src/cpp/ParticleSimulator/Rendering/ParticleStructs.h"
	"src/cpp/ParticleSimulator/Rendering/Renderer.h"
	"src/cpp/ParticleSimulator/Rendering/RenderPass/ComputePass.h"
	"src/cpp/ParticleSimulator/Rendering/RenderPass/GravityViewPass.h"
	"src/cpp/ParticleSimulator/Rendering/RenderPass/ParticleViewPass.h"
	"src/cpp/ParticleSimulator/VKObject/VKContext.h"
	"src/cpp/ParticleSimulator/VKObject/VKDynamic.h"
	"src/cpp/ParticleSimulator/VKObject/VKObject.h"
	"src/cpp/ParticleSimulator/VKObject/VKPtr.h"
	"src/cpp/ParticleSimulator/VKObject/VKSwapchain.h"
	"src/cpp/ParticleSimulator/VKObject/Buffer/VKBuffer.h"
	"src/cpp/ParticleSimulator/VKObject/Buffer/VKBufferBase.h"
	"src/cpp/ParticleSimulator/VKObject/CommandBuffer/VKCommandBuffer.h"
	"src/cpp/ParticleSimulator/VKObject/DescriptorSet/VKDescriptorSet.h"
	"src/cpp/ParticleSimulator/VKObject/DescriptorSet/VKDescriptorSetLayout.h"
	"src/cpp/ParticleSimulator/VKObject/DescriptorSet/VKDescriptorSetLayoutBinding.h"
	"src/cpp/ParticleSimulator/VKObject/DescriptorSet/VKDescriptorSetLayoutInfo.h"
	"src/cpp/ParticleSimulator/VKObject/Fence/VKFence.h"
	"src/cpp/ParticleSimulator/VKObject/Image/VKImage.h"
	"src/cpp/ParticleSimulator/VKObject/Image/VKImageBase.h"
	"src/cpp/ParticleSimulator/VKObject/Image/VKSwapchainImage.h"
	"src/cpp/ParticleSimulator/VKObject/Pipeline/VKComputePipeline.h"
	"src/cpp/ParticleSimulator/VKObject/Pipeline/VKGraphicsPipeline.h"
	"src/cpp/ParticleSimulator/VKObject/Pipeline/VKPipeline.h"
	"src/cpp/ParticleSimulator/VKObject/Pipeline/VKPipelineAttachmentInfo.h"
	"src/cpp/ParticleSimulator/VKObject/Pipeline/VKPipelineLayout.h"
	"src/cpp/ParticleSimulator/VKObject/Pipeline/VKPipelineLayoutInfo.h"
	"src/cpp/ParticleSimulator/VKObject/Pipeline/VKPipelineVertexInputLayoutInfo.h"
	"src/cpp/ParticleSimulator/VKObject/Pipeline/VKPipelineViewport.h"
	"src/cpp/ParticleSimulator/VKObject/Queue/VKQueue.h"
	"src/cpp/ParticleSimulator/VKObject/Sampler/VKSampler.h"
	"src/cpp/ParticleSimulator/VKObject/Semaphore/VKSemaphore.h"
	"src/cpp/ParticleSimulator/VKObject/Shader/VKShader.h"
)

target_shaders(ParticleSimulator shaders "src/glsl"
	"src/glsl/gravityView.comp"
	"src/glsl/particleView.vert"
	"src/glsl/particleView.frag"
	"src/glsl/simulateParticles.comp"
)

set_target_properties(ParticleSimulator PROPERTIES CXX_STANDARD 20)
set_target_properties(ParticleSimulator PROPERTIES CXX_STANDARD_REQUIRED ON)
set_target_properties(ParticleSimulator PROPERTIES CXX_EXTENSIONS OFF)
set_target_properties(ParticleSimulator PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO TRUE)
set_target_properties(ParticleSimulator PROPERTIES INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
set_target_properties(ParticleSimulator PROPERTIES DEBUG_POSTFIX d)

target_compile_definitions(ParticleSimulator PRIVATE GLM_ENABLE_EXPERIMENTAL)
target_compile_definitions(ParticleSimulator PRIVATE GLM_FORCE_DEPTH_ZERO_TO_ONE)
target_compile_definitions(ParticleSimulator PRIVATE GLFW_INCLUDE_NONE)
target_compile_definitions(ParticleSimulator PRIVATE VK_NO_PROTOTYPES)
target_compile_definitions(ParticleSimulator PRIVATE VULKAN_HPP_NO_SETTERS)
target_compile_definitions(ParticleSimulator PRIVATE VULKAN_HPP_NO_NODISCARD_WARNINGS)
target_compile_definitions(ParticleSimulator PRIVATE VULKAN_HPP_NO_SMART_HANDLE)

find_package(VulkanHeaders REQUIRED)
target_link_libraries(ParticleSimulator PRIVATE Vulkan::Headers)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(ParticleSimulator PRIVATE glfw)

find_package(glm CONFIG REQUIRED)
target_link_libraries(ParticleSimulator PRIVATE glm::glm)

find_package(VulkanMemoryAllocator CONFIG REQUIRED)
target_link_libraries(ParticleSimulator PRIVATE GPUOpen::VulkanMemoryAllocator)
