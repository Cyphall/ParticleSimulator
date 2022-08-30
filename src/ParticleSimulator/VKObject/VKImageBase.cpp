#include "VKImageBase.h"

VKImageBase::VKImageBase(vk::Format format, vk::Extent2D extent, vk::ImageLayout layout, int mipLevels, vk::ImageAspectFlags aspect):
	_format(format),
	_extent(extent),
	_currentLayout(layout),
	_mipLevels(mipLevels),
	_aspect(aspect)
{
	
}

void VKImageBase::transitionLayout(vk::CommandBuffer commandBuffer, vk::ImageLayout newLayout)
{
	if (newLayout == _currentLayout)
	{
		return;
	}
	
	vk::ImageMemoryBarrier2 imageMemoryBarrier;
	imageMemoryBarrier.oldLayout = _currentLayout;
	imageMemoryBarrier.newLayout = newLayout;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.image = _handle;
	imageMemoryBarrier.subresourceRange.aspectMask = _aspect;
	imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	imageMemoryBarrier.subresourceRange.levelCount = _mipLevels;
	imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	imageMemoryBarrier.subresourceRange.layerCount = 1;
	
	switch (_currentLayout)
	{
		case vk::ImageLayout::eUndefined:
			imageMemoryBarrier.srcStageMask = vk::PipelineStageFlagBits2::eNone;
			imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits2::eNone;
			break;
		case vk::ImageLayout::eColorAttachmentOptimal:
			imageMemoryBarrier.srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
			imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite;
			break;
		case vk::ImageLayout::ePresentSrcKHR:
			imageMemoryBarrier.srcStageMask = vk::PipelineStageFlagBits2::eNone;
			imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits2::eNone;
			break;
		case vk::ImageLayout::eTransferDstOptimal:
			imageMemoryBarrier.srcStageMask = vk::PipelineStageFlagBits2::eAllTransfer;
			imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits2::eTransferWrite;
			break;
		case vk::ImageLayout::eGeneral:
			imageMemoryBarrier.srcStageMask = vk::PipelineStageFlagBits2::eComputeShader;
			imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits2::eShaderStorageWrite;
			break;
		default:
			throw; // unsupported
	}
	
	switch (newLayout)
	{
		case vk::ImageLayout::eColorAttachmentOptimal:
			imageMemoryBarrier.dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
			imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite;
			break;
		case vk::ImageLayout::ePresentSrcKHR:
			imageMemoryBarrier.dstStageMask = vk::PipelineStageFlagBits2::eNone;
			imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits2::eNone;
			break;
		case vk::ImageLayout::eTransferDstOptimal:
			imageMemoryBarrier.dstStageMask = vk::PipelineStageFlagBits2::eAllTransfer;
			imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits2::eTransferWrite;
			break;
		case vk::ImageLayout::eShaderReadOnlyOptimal:
			imageMemoryBarrier.dstStageMask = vk::PipelineStageFlagBits2::eFragmentShader;
			imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits2::eShaderRead;
			break;
		case vk::ImageLayout::eDepthAttachmentOptimal:
			imageMemoryBarrier.dstStageMask = vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests;
			imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits2::eDepthStencilAttachmentRead | vk::AccessFlagBits2::eDepthStencilAttachmentWrite;
			break;
		case vk::ImageLayout::eGeneral:
			imageMemoryBarrier.dstStageMask = vk::PipelineStageFlagBits2::eComputeShader;
			imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits2::eShaderStorageWrite;
			break;
		default:
			throw; // unsupported
	}
	
	vk::DependencyInfo dependencyInfo;
	dependencyInfo.dependencyFlags = {};
	dependencyInfo.memoryBarrierCount = 0;
	dependencyInfo.pMemoryBarriers = nullptr;
	dependencyInfo.bufferMemoryBarrierCount = 0;
	dependencyInfo.pBufferMemoryBarriers = nullptr;
	dependencyInfo.imageMemoryBarrierCount = 1;
	dependencyInfo.pImageMemoryBarriers = &imageMemoryBarrier;
	
	commandBuffer.pipelineBarrier2(dependencyInfo);
	
	_currentLayout = newLayout;
}

vk::Image VKImageBase::getHandle()
{
	return _handle;
}

vk::ImageView VKImageBase::getView()
{
	return _view;
}

vk::Format VKImageBase::getFormat() const
{
	return _format;
}

vk::Extent2D VKImageBase::getExtent() const
{
	return _extent;
}

vk::ImageLayout VKImageBase::getLayout() const
{
	return _currentLayout;
}