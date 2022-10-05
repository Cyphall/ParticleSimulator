#pragma once

#include <ParticleSimulator/VKObject/Pipeline/VKPipeline.h>

#include <filesystem>
#include <optional>
#include <vulkan/vulkan.hpp>

template<typename T>
class VKDynamic;
class VKSwapchain;
class VKPipelineLayout;

class VKComputePipeline : public VKPipeline
{
public:
	static VKPtr<VKComputePipeline> create(
		VKContext& context,
		const std::filesystem::path& computeShaderFile,
		const VKPtr<VKPipelineLayout>& pipelineLayout);
	static std::unique_ptr<VKDynamic<VKComputePipeline>> createDynamic(
		VKContext& context,
		VKSwapchain& swapchain,
		const std::filesystem::path& computeShaderFile,
		const VKPtr<VKPipelineLayout>& pipelineLayout);
	
	~VKComputePipeline() override;
	
	vk::PipelineBindPoint getPipelineType() override;

private:
	VKComputePipeline(
		VKContext& context,
		const std::filesystem::path& computeShaderFile,
		const VKPtr<VKPipelineLayout>& pipelineLayout);
	
	void createPipeline(const std::filesystem::path& computeShaderFile);
};