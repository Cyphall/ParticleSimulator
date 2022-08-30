#pragma once

#include <filesystem>
#include <optional>
#include <vulkan/vulkan.hpp>

class VKDescriptorSet;

class VKPipeline
{
public:
	virtual ~VKPipeline() = default;
	
	vk::Pipeline getHandle();
	vk::PipelineLayout getPipelineLayout();
	
	void bindDescriptorSet(vk::CommandBuffer commandBuffer, uint32_t set, VKDescriptorSet& descriptorSet);
	void bindDescriptorSet(vk::CommandBuffer commandBuffer, uint32_t set, VKDescriptorSet& descriptorSet, uint32_t dynamicOffset);

protected:
	VKPipeline() = default;
	
	vk::PipelineLayout _pipelineLayout;
	vk::Pipeline _pipeline;
	
	virtual vk::PipelineBindPoint getPipelineType() = 0;
};