#include "VKPipelineAttachmentInfo.h"

void VKPipelineAttachmentInfo::registerColorAttachment(uint32_t slot, vk::Format format)
{
	_colorAttachments.resize(std::max<size_t>(slot + 1, _colorAttachments.size()), vk::Format::eUndefined);
	_colorAttachments.at(slot) = format;
}

void VKPipelineAttachmentInfo::setDepthAttachment(vk::Format format)
{
	_depthAttachment = format;
}

void VKPipelineAttachmentInfo::setStencilAttachment(vk::Format format)
{
	_stencilAttachment = format;
}

vk::PipelineRenderingCreateInfo VKPipelineAttachmentInfo::get() const
{
	vk::PipelineRenderingCreateInfo pipelineRenderingCreateInfo;
	pipelineRenderingCreateInfo.viewMask = 0;
	pipelineRenderingCreateInfo.colorAttachmentCount = _colorAttachments.size();
	pipelineRenderingCreateInfo.pColorAttachmentFormats = _colorAttachments.data();
	pipelineRenderingCreateInfo.depthAttachmentFormat = _depthAttachment;
	pipelineRenderingCreateInfo.stencilAttachmentFormat = _stencilAttachment;
	
	return pipelineRenderingCreateInfo;
}