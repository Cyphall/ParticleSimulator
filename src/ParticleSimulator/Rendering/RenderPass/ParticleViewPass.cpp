#include "ParticleViewPass.h"

#include <ParticleSimulator/RenderContext.h>
#include <ParticleSimulator/VKObject/Buffer/VKBuffer.h>
#include <ParticleSimulator/VKObject/Pipeline/VKPipelineLayoutInfo.h>
#include <ParticleSimulator/VKObject/Pipeline/VKPipelineLayout.h>
#include <ParticleSimulator/VKObject/Pipeline/VKPipelineVertexInputLayoutInfo.h>
#include <ParticleSimulator/VKObject/Pipeline/VKPipelineViewport.h>
#include <ParticleSimulator/VKObject/Pipeline/VKPipelineAttachmentInfo.h>
#include <ParticleSimulator/VKObject/Pipeline/VKGraphicsPipeline.h>
#include <ParticleSimulator/VKObject/CommandBuffer/VKCommandBuffer.h>
#include <ParticleSimulator/VKObject/Image/VKImageBase.h>
#include <ParticleSimulator/Camera.h>

#include <glm/glm.hpp>

ParticleViewPass::ParticleViewPass()
{
	createPipelineLayout();
	createPipeline();
}

ParticleViewPass::~ParticleViewPass()
{
	
}

ParticleViewPass::RenderOutput ParticleViewPass::render(const VKPtr<VKCommandBuffer>& commandBuffer, const ParticleViewPass::RenderInput& input)
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
	renderingInfo.renderArea.extent = vk::Extent2D(input.outputImage->getSize().x, input.outputImage->getSize().y);
	renderingInfo.layerCount = 1;
	renderingInfo.viewMask = 0;
	renderingInfo.colorAttachmentCount = 1;
	renderingInfo.pColorAttachments = &colorAttachment;
	renderingInfo.pDepthAttachment = nullptr;
	renderingInfo.pStencilAttachment = nullptr;
	
	commandBuffer->beginRendering(renderingInfo);
	
	commandBuffer->bindPipeline(_pipeline);
	
	PushConstantData pushConstantData{};
	pushConstantData.vp = input.camera->getProjection() * input.camera->getView();
	commandBuffer->pushConstants(vk::ShaderStageFlagBits::eVertex, pushConstantData);
	
	commandBuffer->bindVertexBuffer(0, *input.particlesBuffer);
	
	commandBuffer->draw(input.particleCount, 0);
	
	commandBuffer->unbindPipeline();
	
	commandBuffer->endRendering();
	
	return {};
}

void ParticleViewPass::createPipelineLayout()
{
	VKPipelineLayoutInfo pipelineLayoutInfo;
	pipelineLayoutInfo.registerPushConstantLayout<PushConstantData>(vk::ShaderStageFlagBits::eVertex);
	
	_pipelineLayout = VKPipelineLayout::create(*RenderContext::vkContext, pipelineLayoutInfo);
}

void ParticleViewPass::createPipeline()
{
	VKPipelineVertexInputLayoutInfo vertexInputLayoutInfo;
	vertexInputLayoutInfo.defineAttribute(0, 0, vk::Format::eR32G32Sfloat, offsetof(ParticleData, position));
	vertexInputLayoutInfo.defineAttribute(0, 1, vk::Format::eR32G32Sfloat, offsetof(ParticleData, velocity));
	vertexInputLayoutInfo.defineSlot(0, sizeof(ParticleData), vk::VertexInputRate::eVertex);
	
	VKPipelineViewport viewport;
	viewport.offset = {0, 0};
	viewport.size = RenderContext::swapchain->getSize();
	viewport.depthRange = {0.0f, 1.0f};
	
	VKPipelineAttachmentInfo pipelineAttachmentInfo;
	pipelineAttachmentInfo.registerColorAttachment(0, RenderContext::swapchain->getFormat());
	
	_pipeline = VKGraphicsPipeline::create(
		*RenderContext::vkContext,
		"resources/shaders/particleView.vert",
		"resources/shaders/particleView.frag",
		vertexInputLayoutInfo,
		vk::PrimitiveTopology::ePointList,
		_pipelineLayout,
		viewport,
		pipelineAttachmentInfo);
}