#include "ParticleViewPass.h"

#include <ParticleSimulator/RenderContext.h>
#include <ParticleSimulator/VKObject/Dynamic/VKDynamicBuffer.h>
#include <ParticleSimulator/Rendering/Renderer.h>
#include <ParticleSimulator/VKObject/Pipeline/VKPipelineVertexInputLayoutInfo.h>
#include <ParticleSimulator/VKObject/Pipeline/VKPipelineLayoutInfo.h>
#include <ParticleSimulator/VKObject/Pipeline/VKGraphicsPipeline.h>
#include <ParticleSimulator/VKObject/Pipeline/VKPipelineViewport.h>
#include <ParticleSimulator/VKObject/VKDescriptorSetLayoutBuilder.h>
#include <ParticleSimulator/VKObject/VKDescriptorSet.h>
#include <ParticleSimulator/VKObject/VKImage.h>
#include <ParticleSimulator/VKObject/VKSwapchainImage.h>
#include <ParticleSimulator/Camera.h>

#include <glm/glm.hpp>

ParticleViewPass::ParticleViewPass()
{
	createPipeline();
}

ParticleViewPass::~ParticleViewPass()
{
	_pipeline.reset();
}

ParticleViewPass::RenderOutput ParticleViewPass::render(vk::CommandBuffer commandBuffer, const ParticleViewPass::RenderInput& input)
{
	vk::ClearValue colorClearValue;
	colorClearValue.color.float32[0] = 0.0f;
	colorClearValue.color.float32[1] = 0.0f;
	colorClearValue.color.float32[2] = 0.0f;
	colorClearValue.color.float32[3] = 1.0f;
	
	vk::RenderingAttachmentInfo colorAttachment;
	colorAttachment.imageView = input.outputImage->getView();
	colorAttachment.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
	colorAttachment.resolveMode = vk::ResolveModeFlagBits::eNone;
	colorAttachment.resolveImageView = nullptr;
	colorAttachment.resolveImageLayout = vk::ImageLayout::eUndefined;
	colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
	colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
	colorAttachment.clearValue = colorClearValue;
	
	vk::RenderingInfo renderingInfo;
	renderingInfo.renderArea.offset = vk::Offset2D(0, 0);
	renderingInfo.renderArea.extent = input.outputImage->getExtent();
	renderingInfo.layerCount = 1;
	renderingInfo.viewMask = 0;
	renderingInfo.colorAttachmentCount = 1;
	renderingInfo.pColorAttachments = &colorAttachment;
	renderingInfo.pDepthAttachment = nullptr;
	renderingInfo.pStencilAttachment = nullptr;
	
	commandBuffer.beginRendering(renderingInfo);
	
	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, _pipeline->getHandle());
	
	PushConstantData pushConstantData{};
	pushConstantData.vp = input.camera->getProjection() * input.camera->getView();
	commandBuffer.pushConstants(_pipeline->getPipelineLayout(), vk::ShaderStageFlagBits::eVertex, 0, sizeof(PushConstantData), &pushConstantData);
	
	commandBuffer.bindVertexBuffers(0, input.particlesBuffer->getHandle(), {0});
	
	commandBuffer.draw(input.particleCount, 1, 0, 0);
	
	commandBuffer.endRendering();
	
	return {};
}

void ParticleViewPass::createPipeline()
{
	VKPipelineVertexInputLayoutInfo vertexInputLayoutInfo;
	vertexInputLayoutInfo.defineAttribute(0, 0, vk::Format::eR32G32Sfloat, offsetof(ParticleData, position));
	vertexInputLayoutInfo.defineAttribute(0, 1, vk::Format::eR32G32Sfloat, offsetof(ParticleData, velocity));
	vertexInputLayoutInfo.defineSlot(0, sizeof(ParticleData), vk::VertexInputRate::eVertex);
	
	VKPipelineLayoutInfo pipelineLayoutInfo;
	pipelineLayoutInfo.registerPushConstantLayout<PushConstantData>(vk::ShaderStageFlagBits::eVertex);
	
	vk::Extent2D swapchainExtent = RenderContext::swapchain->getExtent();
	
	VKPipelineViewport viewport;
	viewport.offset = {0, 0};
	viewport.size = {swapchainExtent.width, swapchainExtent.height};
	viewport.depthRange = {0.0f, 1.0f};
	
	_pipeline = std::make_unique<VKGraphicsPipeline>(
		"resources/shaders/particleView.vert",
		"resources/shaders/particleView.frag",
		vertexInputLayoutInfo,
		vk::PrimitiveTopology::ePointList,
		pipelineLayoutInfo,
		viewport,
		nullptr);
}