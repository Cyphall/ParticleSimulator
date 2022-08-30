#include "VKPipelineLayoutInfo.h"

#include <ParticleSimulator/VKObject/VKDescriptorSetLayout.h>

void VKPipelineLayoutInfo::registerDescriptorSetLayout(VKDescriptorSetLayout& descriptorSetLayout)
{
	_descriptorSetsLayout.push_back(descriptorSetLayout.getHandle());
}

vk::PipelineLayoutCreateInfo VKPipelineLayoutInfo::get() const
{
	vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
	pipelineLayoutCreateInfo.setLayoutCount = _descriptorSetsLayout.size();
	pipelineLayoutCreateInfo.pSetLayouts = _descriptorSetsLayout.data();
	
	if (_pushConstantRange.has_value())
	{
		pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		pipelineLayoutCreateInfo.pPushConstantRanges = &_pushConstantRange.value();
	}
	else
	{
		pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
		pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
	}
	
	return pipelineLayoutCreateInfo;
}