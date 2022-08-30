#include "ComputePass.h"

#include <ParticleSimulator/RenderContext.h>
#include <ParticleSimulator/VKObject/Dynamic/VKDynamicBuffer.h>
#include <ParticleSimulator/VKObject/Pipeline/VKPipelineLayoutInfo.h>
#include <ParticleSimulator/VKObject/Pipeline/VKComputePipeline.h>
#include <ParticleSimulator/VKObject/VKDescriptorSetLayoutBuilder.h>
#include <ParticleSimulator/VKObject/VKDescriptorSet.h>

ComputePass::ComputePass()
{
	createDescriptorSetsLayout();
	createDescriptorSets();
	createPipeline();
}

ComputePass::~ComputePass()
{
	_pipeline.reset();
	_particlesDescriptorSet.reset();
	_particlesDescriptorSetLayout.reset();
}

ComputePass::RenderOutput ComputePass::render(vk::CommandBuffer commandBuffer, const ComputePass::RenderInput& input)
{
	commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, _pipeline->getHandle());
	
	_particlesDescriptorSet->bindBuffer(0, input.particlesInputBuffer->getHandle());
	_particlesDescriptorSet->bindBuffer(1, input.particlesOutputBuffer->getHandle());
	_pipeline->bindDescriptorSet(commandBuffer, 0, *_particlesDescriptorSet);
	
	PushConstantData pushConstantData{};
	pushConstantData.deltaTime = input.deltaTime;
	pushConstantData.particleCount = input.particleCount;
	pushConstantData.cursorGravityEnabled = input.cursorGravityEnabled;
	pushConstantData.cursorGravityPos = input.cursorGravityPos;
	pushConstantData.particlesGravityEnabled = input.particlesGravityEnabled;
	commandBuffer.pushConstants(_pipeline->getPipelineLayout(), vk::ShaderStageFlagBits::eCompute, 0, sizeof(PushConstantData), &pushConstantData);
	
	commandBuffer.dispatch(std::max<uint32_t>(std::ceil(input.particleCount / 1024.0), 1), 1, 1);
	
	return {};
}

void ComputePass::createDescriptorSetsLayout()
{
	_particlesDescriptorSetLayout = VKDescriptorSetLayoutBuilder()
		.registerBinding(0, vk::DescriptorType::eStorageBuffer, 1)
		.registerBinding(1, vk::DescriptorType::eStorageBuffer, 1)
		.build();
}

void ComputePass::createDescriptorSets()
{
	_particlesDescriptorSet = std::make_unique<VKDescriptorSet>(*_particlesDescriptorSetLayout);
}

void ComputePass::createPipeline()
{
	VKPipelineLayoutInfo descriptorLayout;
	descriptorLayout.registerDescriptorSetLayout(*_particlesDescriptorSetLayout);
	descriptorLayout.registerPushConstantLayout<PushConstantData>(vk::ShaderStageFlagBits::eCompute);
	
	_pipeline = std::make_unique<VKComputePipeline>(
		"resources/shaders/simulateParticles.comp",
		descriptorLayout);
}