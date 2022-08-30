#include "VKComputePipeline.h"

#include <ParticleSimulator/RenderContext.h>
#include <ParticleSimulator/VKObject/VKShader.h>
#include <ParticleSimulator/VKObject/VKSwapchain.h>
#include <ParticleSimulator/VKObject/VKContext.h>
#include <ParticleSimulator/VKObject/Pipeline/VKPipelineLayoutInfo.h>

VKComputePipeline::VKComputePipeline(const std::filesystem::path& computeShaderFile, const VKPipelineLayoutInfo& descriptorLayout)
{
	createPipelineLayout(descriptorLayout);
	createPipeline(computeShaderFile);
}

VKComputePipeline::~VKComputePipeline()
{
	RenderContext::vkContext->getDevice().destroyPipelineLayout(_pipelineLayout);
	RenderContext::vkContext->getDevice().destroyPipeline(_pipeline);
}

void VKComputePipeline::createPipelineLayout(const VKPipelineLayoutInfo& descriptorLayout)
{
	_pipelineLayout = RenderContext::vkContext->getDevice().createPipelineLayout(descriptorLayout.get());
}

void VKComputePipeline::createPipeline(const std::filesystem::path& computeShaderFile)
{
	VKShader computeShader(computeShaderFile);
	
	vk::PipelineShaderStageCreateInfo shaderStageCreateInfos;
	shaderStageCreateInfos.stage = vk::ShaderStageFlagBits::eCompute;
	shaderStageCreateInfos.module = computeShader.getHandle();
	shaderStageCreateInfos.pName = "main";
	
	vk::ComputePipelineCreateInfo pipelineCreateInfo;
	pipelineCreateInfo.stage = shaderStageCreateInfos;
	pipelineCreateInfo.layout = _pipelineLayout;
	
	_pipeline = RenderContext::vkContext->getDevice().createComputePipeline(VK_NULL_HANDLE, pipelineCreateInfo).value;
}

vk::PipelineBindPoint VKComputePipeline::getPipelineType()
{
	return vk::PipelineBindPoint::eCompute;
}