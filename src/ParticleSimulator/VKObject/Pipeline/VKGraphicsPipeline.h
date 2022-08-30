#pragma once

#include <ParticleSimulator/VKObject/Pipeline/VKPipeline.h>

#include <filesystem>
#include <optional>
#include <vulkan/vulkan.hpp>

class VKPipelineVertexInputLayoutInfo;
class VKPipelineLayoutInfo;
struct VKPipelineViewport;
class VKImage;

class VKGraphicsPipeline : public VKPipeline
{
public:
	VKGraphicsPipeline(
		const std::filesystem::path& vertexShaderFile,
		const std::filesystem::path& fragmentShaderFile,
		const VKPipelineVertexInputLayoutInfo& vertexInputLayoutInfo,
		vk::PrimitiveTopology vertexTopology,
		const VKPipelineLayoutInfo& pipelineLayoutInfo,
		const VKPipelineViewport& viewport,
		const VKImage* depthMap);
	
	~VKGraphicsPipeline() override;
	
protected:
	vk::PipelineBindPoint getPipelineType() override;

private:
	void createPipelineLayout(const VKPipelineLayoutInfo& pipelineLayoutInfo);
	void createPipeline(const std::filesystem::path& vertexShaderFile,
						const std::filesystem::path& fragmentShaderFile,
						const VKPipelineVertexInputLayoutInfo& vertexInputLayoutInfo,
						vk::PrimitiveTopology vertexTopology,
						const VKPipelineViewport& viewport,
						const VKImage* depthMap);
};