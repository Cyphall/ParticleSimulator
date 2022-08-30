#include "VKPipeline.h"

#include <ParticleSimulator/VKObject/VKDescriptorSet.h>

vk::Pipeline VKPipeline::getHandle()
{
	return _pipeline;
}

vk::PipelineLayout VKPipeline::getPipelineLayout()
{
	return _pipelineLayout;
}

void VKPipeline::bindDescriptorSet(vk::CommandBuffer commandBuffer, uint32_t set, VKDescriptorSet& descriptorSet)
{
	commandBuffer.bindDescriptorSets(
		getPipelineType(),
		_pipelineLayout,
		set,
		descriptorSet.getHandle(),
		{});
}

void VKPipeline::bindDescriptorSet(vk::CommandBuffer commandBuffer, uint32_t set, VKDescriptorSet& descriptorSet, uint32_t dynamicOffset)
{
	commandBuffer.bindDescriptorSets(
		getPipelineType(),
		_pipelineLayout,
		set,
		descriptorSet.getHandle(),
		dynamicOffset);
}