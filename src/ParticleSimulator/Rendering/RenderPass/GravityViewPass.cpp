#include "GravityViewPass.h"

#include <ParticleSimulator/RenderContext.h>
#include <ParticleSimulator/VKObject/Dynamic/VKDynamicBuffer.h>
#include <ParticleSimulator/VKObject/Pipeline/VKPipelineLayoutInfo.h>
#include <ParticleSimulator/VKObject/Pipeline/VKComputePipeline.h>
#include <ParticleSimulator/VKObject/VKDescriptorSetLayoutBuilder.h>
#include <ParticleSimulator/VKObject/VKDescriptorSet.h>
#include <ParticleSimulator/VKObject/VKImage.h>
#include <ParticleSimulator/Camera.h>

GravityViewPass::GravityViewPass()
{
	createDescriptorSetsLayout();
	createDescriptorSets();
	createPipeline();
}

GravityViewPass::~GravityViewPass()
{
	_pipeline.reset();
	_particlesDescriptorSet.reset();
	_particlesDescriptorSetLayout.reset();
}

GravityViewPass::RenderOutput GravityViewPass::render(vk::CommandBuffer commandBuffer, const GravityViewPass::RenderInput& input)
{
	commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, _pipeline->getHandle());
	
	_particlesDescriptorSet->bindBuffer(0, input.particlesBuffer->getHandle());
	_particlesDescriptorSet->bindImage(1, input.outputImage->getView(), input.outputImage->getLayout());
	_pipeline->bindDescriptorSet(commandBuffer, 0, *_particlesDescriptorSet);
	
	PushConstantData pushConstantData{};
	pushConstantData.vp = input.camera->getProjection() * input.camera->getView();
	pushConstantData.particleCount = input.particleCount;
	commandBuffer.pushConstants(_pipeline->getPipelineLayout(), vk::ShaderStageFlagBits::eCompute, 0, sizeof(PushConstantData), &pushConstantData);
	
	commandBuffer.dispatch(
		std::max<uint32_t>(std::ceil(input.outputImage->getExtent().width / 32.0), 1),
		std::max<uint32_t>(std::ceil(input.outputImage->getExtent().height / 32.0), 1),
		1
	);
	
	return {};
}

void GravityViewPass::createDescriptorSetsLayout()
{
	_particlesDescriptorSetLayout = VKDescriptorSetLayoutBuilder()
		.registerBinding(0, vk::DescriptorType::eStorageBuffer, 1)
		.registerBinding(1, vk::DescriptorType::eStorageImage, 1)
		.build();
}

void GravityViewPass::createDescriptorSets()
{
	_particlesDescriptorSet = std::make_unique<VKDescriptorSet>(*_particlesDescriptorSetLayout);
}

void GravityViewPass::createPipeline()
{
	VKPipelineLayoutInfo descriptorLayout;
	descriptorLayout.registerDescriptorSetLayout(*_particlesDescriptorSetLayout);
	descriptorLayout.registerPushConstantLayout<PushConstantData>(vk::ShaderStageFlagBits::eCompute);
	
	_pipeline = std::make_unique<VKComputePipeline>(
		"resources/shaders/gravityView.comp",
		descriptorLayout);
}