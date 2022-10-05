#include "VKPipelineLayout.h"

#include <ParticleSimulator/VKObject/VKContext.h>
#include <ParticleSimulator/VKObject/VKDynamic.h>
#include <ParticleSimulator/VKObject/Pipeline/VKPipelineLayoutInfo.h>

VKPtr<VKPipelineLayout> VKPipelineLayout::create(VKContext& context, const VKPipelineLayoutInfo& pipelineLayoutInfo)
{
	return VKPtr<VKPipelineLayout>(new VKPipelineLayout(context, pipelineLayoutInfo));
}

std::unique_ptr<VKDynamic<VKPipelineLayout>> VKPipelineLayout::createDynamic(VKContext& context, VKSwapchain& swapchain, const VKPipelineLayoutInfo& pipelineLayoutInfo)
{
	return std::unique_ptr<VKDynamic<VKPipelineLayout>>(new VKDynamic<VKPipelineLayout>(context, swapchain, pipelineLayoutInfo));
}

VKPipelineLayout::VKPipelineLayout(VKContext& context, const VKPipelineLayoutInfo& pipelineLayoutInfo):
	VKObject(context)
{
	vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
	pipelineLayoutCreateInfo.setLayoutCount = pipelineLayoutInfo._vkDescriptorSetsLayouts.size();
	pipelineLayoutCreateInfo.pSetLayouts = pipelineLayoutInfo._vkDescriptorSetsLayouts.data();
	
	_descriptorSetsLayouts = pipelineLayoutInfo._descriptorSetsLayouts;
	
	if (pipelineLayoutInfo._pushConstantRange.has_value())
	{
		pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
		pipelineLayoutCreateInfo.pPushConstantRanges = &pipelineLayoutInfo._pushConstantRange.value();
	}
	else
	{
		pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
		pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;
	}
	
	_pipelineLayout = _context.getDevice().createPipelineLayout(pipelineLayoutCreateInfo);
}

VKPipelineLayout::~VKPipelineLayout()
{
	_context.getDevice().destroyPipelineLayout(_pipelineLayout);
}

vk::PipelineLayout VKPipelineLayout::getHandle()
{
	return _pipelineLayout;
}