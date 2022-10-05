#include "GravityViewPass.h"

#include <ParticleSimulator/RenderContext.h>
#include <ParticleSimulator/VKObject/Buffer/VKBuffer.h>
#include <ParticleSimulator/VKObject/Pipeline/VKPipelineLayoutInfo.h>
#include <ParticleSimulator/VKObject/Pipeline/VKPipelineLayout.h>
#include <ParticleSimulator/VKObject/Pipeline/VKComputePipeline.h>
#include <ParticleSimulator/VKObject/DescriptorSet/VKDescriptorSetLayoutInfo.h>
#include <ParticleSimulator/VKObject/DescriptorSet/VKDescriptorSetLayout.h>
#include <ParticleSimulator/VKObject/DescriptorSet/VKDescriptorSet.h>
#include <ParticleSimulator/VKObject/CommandBuffer/VKCommandBuffer.h>
#include <ParticleSimulator/VKObject/Image/VKImageBase.h>
#include <ParticleSimulator/Camera.h>

GravityViewPass::GravityViewPass()
{
	createDescriptorSetsLayout();
	createDescriptorSets();
	createPipelineLayout();
	createPipeline();
}

GravityViewPass::~GravityViewPass()
{
	
}

GravityViewPass::RenderOutput GravityViewPass::render(const VKPtr<VKCommandBuffer>& commandBuffer, const GravityViewPass::RenderInput& input)
{
	commandBuffer->bindPipeline(_pipeline);
	
	_particlesDescriptorSet->bindBuffer(0, *input.particlesBuffer);
	_particlesDescriptorSet->bindImage(1, input.outputImage);
	commandBuffer->bindDescriptorSet(0, _particlesDescriptorSet);
	
	PushConstantData pushConstantData{};
	pushConstantData.vp = input.camera->getProjection() * input.camera->getView();
	pushConstantData.particleCount = input.particleCount;
	commandBuffer->pushConstants(vk::ShaderStageFlagBits::eCompute, pushConstantData);
	
	commandBuffer->dispatch({
		std::max<uint32_t>(std::ceil(input.outputImage->getSize().x / 32.0), 1),
		std::max<uint32_t>(std::ceil(input.outputImage->getSize().y / 32.0), 1),
		1});
	
	commandBuffer->unbindPipeline();
	
	return {};
}

void GravityViewPass::createDescriptorSetsLayout()
{
	VKDescriptorSetLayoutInfo descriptorSetLayoutInfo;
	descriptorSetLayoutInfo.registerBinding(0, vk::DescriptorType::eStorageBuffer, 1);
	descriptorSetLayoutInfo.registerBinding(1, vk::DescriptorType::eStorageImage, 1);
	
	_particlesDescriptorSetLayout = VKDescriptorSetLayout::create(*RenderContext::vkContext, descriptorSetLayoutInfo);
}

void GravityViewPass::createDescriptorSets()
{
	_particlesDescriptorSet = VKDescriptorSet::create(*RenderContext::vkContext, _particlesDescriptorSetLayout);
}

void GravityViewPass::createPipelineLayout()
{
	VKPipelineLayoutInfo pipelineLayoutInfo;
	pipelineLayoutInfo.registerDescriptorSetLayout(_particlesDescriptorSetLayout);
	pipelineLayoutInfo.registerPushConstantLayout<PushConstantData>(vk::ShaderStageFlagBits::eCompute);
	
	_pipelineLayout = VKPipelineLayout::create(*RenderContext::vkContext, pipelineLayoutInfo);
}

void GravityViewPass::createPipeline()
{
	_pipeline = VKComputePipeline::create(
		*RenderContext::vkContext,
		"resources/shaders/gravityView.comp",
		_pipelineLayout);
}