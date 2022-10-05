#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>

class VKPipelineAttachmentInfo
{
public:
	void registerColorAttachment(uint32_t slot, vk::Format format);
	void setDepthAttachment(vk::Format format);
	void setStencilAttachment(vk::Format format);
	
	vk::PipelineRenderingCreateInfo get() const;

private:
	std::vector<vk::Format> _colorAttachments;
	vk::Format _depthAttachment = vk::Format::eUndefined;
	vk::Format _stencilAttachment = vk::Format::eUndefined;
};