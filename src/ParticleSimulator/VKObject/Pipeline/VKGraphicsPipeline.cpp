#include "VKGraphicsPipeline.h"

#include <ParticleSimulator/RenderContext.h>
#include <ParticleSimulator/VKObject/VKShader.h>
#include <ParticleSimulator/VKObject/VKSwapchain.h>
#include <ParticleSimulator/VKObject/VKContext.h>
#include <ParticleSimulator/VKObject/Pipeline/VKPipelineVertexInputLayoutInfo.h>
#include <ParticleSimulator/VKObject/Pipeline/VKPipelineLayoutInfo.h>
#include <ParticleSimulator/VKObject/Pipeline/VKPipelineViewport.h>
#include <ParticleSimulator/VKObject/VKImage.h>

VKGraphicsPipeline::VKGraphicsPipeline(const std::filesystem::path& vertexShaderFile,
									   const std::filesystem::path& fragmentShaderFile,
									   const VKPipelineVertexInputLayoutInfo& vertexInputLayoutInfo,
									   vk::PrimitiveTopology vertexTopology,
									   const VKPipelineLayoutInfo& pipelineLayoutInfo,
									   const VKPipelineViewport& viewport,
									   const VKImage* depthMap)
{
	createPipelineLayout(pipelineLayoutInfo);
	createPipeline(
		vertexShaderFile,
		fragmentShaderFile,
		vertexInputLayoutInfo,
		vertexTopology,
		viewport,
		depthMap);
}

VKGraphicsPipeline::~VKGraphicsPipeline()
{
	RenderContext::vkContext->getDevice().destroyPipelineLayout(_pipelineLayout);
	RenderContext::vkContext->getDevice().destroyPipeline(_pipeline);
}

void VKGraphicsPipeline::createPipelineLayout(const VKPipelineLayoutInfo& pipelineLayoutInfo)
{
	_pipelineLayout = RenderContext::vkContext->getDevice().createPipelineLayout(pipelineLayoutInfo.get());
}

void VKGraphicsPipeline::createPipeline(
	const std::filesystem::path& vertexShaderFile,
	const std::filesystem::path& fragmentShaderFile,
	const VKPipelineVertexInputLayoutInfo& vertexInputLayoutInfo,
	vk::PrimitiveTopology vertexTopology,
	const VKPipelineViewport& viewport,
	const VKImage* depthMap)
{
	VKShader vertexShader(vertexShaderFile);
	VKShader fragmentShader(fragmentShaderFile);
	
	vk::PipelineShaderStageCreateInfo shaderCreateInfos[2];
	
	shaderCreateInfos[0].stage = vk::ShaderStageFlagBits::eVertex;
	shaderCreateInfos[0].module = vertexShader.getHandle();
	shaderCreateInfos[0].pName = "main";
	
	shaderCreateInfos[1].stage = vk::ShaderStageFlagBits::eFragment;
	shaderCreateInfos[1].module = fragmentShader.getHandle();
	shaderCreateInfos[1].pName = "main";
	
	vk::PipelineVertexInputStateCreateInfo vertexInputState = vertexInputLayoutInfo.get();
	
	vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
	inputAssembly.topology = vertexTopology;
	inputAssembly.primitiveRestartEnable = VK_FALSE;
	
	vk::Viewport vkViewport;
	vkViewport.x = static_cast<float>(viewport.offset.x);
	vkViewport.y = static_cast<float>(viewport.offset.y);
	vkViewport.width = static_cast<float>(viewport.size.x);
	vkViewport.height = static_cast<float>(viewport.size.y);
	vkViewport.minDepth = viewport.depthRange.x;
	vkViewport.maxDepth = viewport.depthRange.y;
	
	vk::Rect2D scissor;
	scissor.offset.x = viewport.offset.x;
	scissor.offset.y = viewport.offset.y;
	scissor.extent.width = viewport.size.x;
	scissor.extent.height = viewport.size.y;
	
	vk::PipelineViewportStateCreateInfo viewportState;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &vkViewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;
	
	vk::PipelineRasterizationStateCreateInfo rasterizer;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = vk::PolygonMode::eFill;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = vk::CullModeFlagBits::eBack;
	rasterizer.frontFace = vk::FrontFace::eCounterClockwise;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
	
	vk::PipelineMultisampleStateCreateInfo multisampling;
	multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional
	
	vk::PipelineColorBlendAttachmentState colorBlendAttachment;
	colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne; // Optional
	colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero; // Optional
	colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero; // Optional
	colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd; // Optional
	
	vk::PipelineColorBlendStateCreateInfo colorBlending;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = vk::LogicOp::eCopy; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional
	
	vk::Format swapchainImageFormat = RenderContext::swapchain->getFormat();
	
	vk::PipelineRenderingCreateInfo pipelineRenderingCreateInfo;
	pipelineRenderingCreateInfo.viewMask = 0;
	pipelineRenderingCreateInfo.colorAttachmentCount = 1;
	pipelineRenderingCreateInfo.pColorAttachmentFormats = &swapchainImageFormat;
	pipelineRenderingCreateInfo.depthAttachmentFormat = depthMap != nullptr ? depthMap->getFormat() : vk::Format::eUndefined;
	pipelineRenderingCreateInfo.stencilAttachmentFormat = vk::Format::eUndefined;
	
	vk::PipelineDepthStencilStateCreateInfo depthState;
	if (depthMap != nullptr)
	{
		depthState.depthTestEnable = true;
		depthState.depthWriteEnable = true;
	}
	else
	{
		depthState.depthTestEnable = false; // Optional
		depthState.depthWriteEnable = false; // Optional
	}
	depthState.depthCompareOp = vk::CompareOp::eLess;
	depthState.depthBoundsTestEnable = false;
	depthState.minDepthBounds = 0.0f; // Optional
	depthState.maxDepthBounds = 1.0f; // Optional
	depthState.stencilTestEnable = false;
	depthState.front = vk::StencilOpState(); // Optional
	depthState.back = vk::StencilOpState(); // Optional
	
	vk::GraphicsPipelineCreateInfo pipelineCreateInfo;
	pipelineCreateInfo.stageCount = 2;
	pipelineCreateInfo.pStages = shaderCreateInfos;
	pipelineCreateInfo.pVertexInputState = &vertexInputState;
	pipelineCreateInfo.pInputAssemblyState = &inputAssembly;
	pipelineCreateInfo.pViewportState = &viewportState;
	pipelineCreateInfo.pRasterizationState = &rasterizer;
	pipelineCreateInfo.pMultisampleState = &multisampling;
	pipelineCreateInfo.pDepthStencilState = &depthState; // Optional
	pipelineCreateInfo.pColorBlendState = &colorBlending;
	pipelineCreateInfo.pDynamicState = nullptr; // Optional
	pipelineCreateInfo.layout = _pipelineLayout;
	pipelineCreateInfo.renderPass = nullptr;
	pipelineCreateInfo.subpass = 0;
	pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineCreateInfo.basePipelineIndex = -1; // Optional
	pipelineCreateInfo.pNext = &pipelineRenderingCreateInfo;
	
	_pipeline = RenderContext::vkContext->getDevice().createGraphicsPipeline(VK_NULL_HANDLE, pipelineCreateInfo).value;
}

vk::PipelineBindPoint VKGraphicsPipeline::getPipelineType()
{
	return vk::PipelineBindPoint::eGraphics;
}