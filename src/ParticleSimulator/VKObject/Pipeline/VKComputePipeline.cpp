#include "VKComputePipeline.h"

#include <ParticleSimulator/VKObject/VKContext.h>
#include <ParticleSimulator/VKObject/VKDynamic.h>
#include <ParticleSimulator/VKObject/Pipeline/VKPipelineLayout.h>
#include <ParticleSimulator/VKObject/Shader/VKShader.h>

VKPtr<VKComputePipeline> VKComputePipeline::create(
	VKContext& context,
	const std::filesystem::path& computeShaderFile,
	const VKPtr<VKPipelineLayout>& pipelineLayout)
{
	return VKPtr<VKComputePipeline>(new VKComputePipeline(
		context,
		computeShaderFile,
		pipelineLayout));
}

std::unique_ptr<VKDynamic<VKComputePipeline>> VKComputePipeline::createDynamic(
	VKContext& context,
	VKSwapchain& swapchain,
	const std::filesystem::path& computeShaderFile,
	const VKPtr<VKPipelineLayout>& pipelineLayout)
{
	return std::unique_ptr<VKDynamic<VKComputePipeline>>(new VKDynamic<VKComputePipeline>(
		context,
		swapchain,
		computeShaderFile,
		pipelineLayout));
}

VKComputePipeline::VKComputePipeline(
	VKContext& context,
	const std::filesystem::path& computeShaderFile,
	const VKPtr<VKPipelineLayout>& pipelineLayout):
	VKPipeline(context, pipelineLayout)
{
	createPipeline(computeShaderFile);
}

VKComputePipeline::~VKComputePipeline()
{
	_context.getDevice().destroyPipeline(_pipeline);
}

void VKComputePipeline::createPipeline(const std::filesystem::path& computeShaderFile)
{
	VKPtr<VKShader> computeShader = VKShader::create(_context, computeShaderFile);
	
	vk::PipelineShaderStageCreateInfo shaderStageCreateInfos;
	shaderStageCreateInfos.stage = vk::ShaderStageFlagBits::eCompute;
	shaderStageCreateInfos.module = computeShader->getHandle();
	shaderStageCreateInfos.pName = "main";
	
	vk::ComputePipelineCreateInfo pipelineCreateInfo;
	pipelineCreateInfo.stage = shaderStageCreateInfos;
	pipelineCreateInfo.layout = _pipelineLayout->getHandle();
	
	_pipeline = _context.getDevice().createComputePipeline(VK_NULL_HANDLE, pipelineCreateInfo).value;
}

vk::PipelineBindPoint VKComputePipeline::getPipelineType()
{
	return vk::PipelineBindPoint::eCompute;
}