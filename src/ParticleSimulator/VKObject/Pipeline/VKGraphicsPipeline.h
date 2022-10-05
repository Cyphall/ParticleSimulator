#pragma once

#include <ParticleSimulator/VKObject/Pipeline/VKPipeline.h>

#include <filesystem>
#include <vulkan/vulkan.hpp>

template<typename T>
class VKDynamic;
class VKSwapchain;
class VKPipelineVertexInputLayoutInfo;
class VKPipelineLayout;
struct VKPipelineViewport;
class VKPipelineAttachmentInfo;

class VKGraphicsPipeline : public VKPipeline
{
public:
	static VKPtr<VKGraphicsPipeline> create(
		VKContext& context,
		const std::filesystem::path& vertexShaderFile,
		const std::filesystem::path& fragmentShaderFile,
		const VKPipelineVertexInputLayoutInfo& vertexInputLayoutInfo,
		vk::PrimitiveTopology vertexTopology,
		const VKPtr<VKPipelineLayout>& pipelineLayout,
		const VKPipelineViewport& viewport,
		const VKPipelineAttachmentInfo& pipelineAttachmentInfo);
	static std::unique_ptr<VKDynamic<VKGraphicsPipeline>> createDynamic(
		VKContext& context,
		VKSwapchain& swapchain,
		const std::filesystem::path& vertexShaderFile,
		const std::filesystem::path& fragmentShaderFile,
		const VKPipelineVertexInputLayoutInfo& vertexInputLayoutInfo,
		vk::PrimitiveTopology vertexTopology,
		const VKPtr<VKPipelineLayout>& pipelineLayout,
		const VKPipelineViewport& viewport,
		const VKPipelineAttachmentInfo& pipelineAttachmentInfo);
	
	~VKGraphicsPipeline() override;
	
	vk::PipelineBindPoint getPipelineType() override;
	
private:
	VKGraphicsPipeline(
		VKContext& context,
		const std::filesystem::path& vertexShaderFile,
		const std::filesystem::path& fragmentShaderFile,
		const VKPipelineVertexInputLayoutInfo& vertexInputLayoutInfo,
		vk::PrimitiveTopology vertexTopology,
		const VKPtr<VKPipelineLayout>& pipelineLayout,
		const VKPipelineViewport& viewport,
		const VKPipelineAttachmentInfo& pipelineAttachmentInfo);
	
	void createPipeline(
		const std::filesystem::path& vertexShaderFile,
		const std::filesystem::path& fragmentShaderFile,
		const VKPipelineVertexInputLayoutInfo& vertexInputLayoutInfo,
		vk::PrimitiveTopology vertexTopology,
		const VKPipelineViewport& viewport,
		const VKPipelineAttachmentInfo& pipelineAttachmentInfo);
};