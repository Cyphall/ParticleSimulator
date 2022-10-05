#include "ComputePass.h"

#include <ParticleSimulator/RenderContext.h>
#include <ParticleSimulator/VKObject/Buffer/VKBuffer.h>
#include <ParticleSimulator/VKObject/Pipeline/VKPipelineLayoutInfo.h>
#include <ParticleSimulator/VKObject/Pipeline/VKPipelineLayout.h>
#include <ParticleSimulator/VKObject/Pipeline/VKComputePipeline.h>
#include <ParticleSimulator/VKObject/DescriptorSet/VKDescriptorSetLayoutInfo.h>
#include <ParticleSimulator/VKObject/DescriptorSet/VKDescriptorSetLayout.h>
#include <ParticleSimulator/VKObject/DescriptorSet/VKDescriptorSet.h>
#include <ParticleSimulator/VKObject/CommandBuffer/VKCommandBuffer.h>

ComputePass::ComputePass()
{
	createDescriptorSetsLayout();
	createDescriptorSets();
	createPipelineLayout();
	createPipeline();
}

ComputePass::~ComputePass()
{
	
}

ComputePass::RenderOutput ComputePass::render(const VKPtr<VKCommandBuffer>& commandBuffer, const ComputePass::RenderInput& input)
{
	commandBuffer->bindPipeline(_pipeline);
	
	_particlesDescriptorSet->bindBuffer(0, *input.particlesInputBuffer);
	_particlesDescriptorSet->bindBuffer(1, *input.particlesOutputBuffer);
	commandBuffer->bindDescriptorSet(0, _particlesDescriptorSet);
	
	PushConstantData pushConstantData{};
	pushConstantData.deltaTime = input.deltaTime;
	pushConstantData.particleCount = input.particleCount;
	pushConstantData.cursorGravityEnabled = input.cursorGravityEnabled;
	pushConstantData.cursorGravityPos = input.cursorGravityPos;
	pushConstantData.particlesGravityEnabled = input.particlesGravityEnabled;
	commandBuffer->pushConstants(vk::ShaderStageFlagBits::eCompute, pushConstantData);
	
	commandBuffer->dispatch({std::max<uint32_t>(std::ceil(input.particleCount / 1024.0), 1), 1, 1});
	
	commandBuffer->unbindPipeline();
	
	return {};
}

void ComputePass::createDescriptorSetsLayout()
{
	VKDescriptorSetLayoutInfo descriptorSetLayoutInfo;
	descriptorSetLayoutInfo.registerBinding(0, vk::DescriptorType::eStorageBuffer, 1);
	descriptorSetLayoutInfo.registerBinding(1, vk::DescriptorType::eStorageBuffer, 1);
	
	_particlesDescriptorSetLayout = VKDescriptorSetLayout::create(*RenderContext::vkContext, descriptorSetLayoutInfo);
}

void ComputePass::createDescriptorSets()
{
	_particlesDescriptorSet = VKDescriptorSet::create(*RenderContext::vkContext, _particlesDescriptorSetLayout);
}

void ComputePass::createPipelineLayout()
{
	VKPipelineLayoutInfo pipelineLayoutInfo;
	pipelineLayoutInfo.registerDescriptorSetLayout(_particlesDescriptorSetLayout);
	pipelineLayoutInfo.registerPushConstantLayout<PushConstantData>(vk::ShaderStageFlagBits::eCompute);
	
	_pipelineLayout = VKPipelineLayout::create(*RenderContext::vkContext, pipelineLayoutInfo);
}

void ComputePass::createPipeline()
{
	_pipeline = VKComputePipeline::create(
		*RenderContext::vkContext,
		"resources/shaders/simulateParticles.comp",
		_pipelineLayout);
}