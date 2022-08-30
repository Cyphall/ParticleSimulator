#pragma once

#include <ParticleSimulator/VKObject/Pipeline/VKPipeline.h>

#include <filesystem>
#include <optional>
#include <vulkan/vulkan.hpp>

class VKPipelineLayoutInfo;

class VKComputePipeline : public VKPipeline
{
public:
	VKComputePipeline(const std::filesystem::path& computeShaderFile, const VKPipelineLayoutInfo& descriptorLayout);
	
	~VKComputePipeline() override;
	
protected:
	vk::PipelineBindPoint getPipelineType() override;

private:
	void createPipelineLayout(const VKPipelineLayoutInfo& descriptorLayout);
	void createPipeline(const std::filesystem::path& computeShaderFile);
};