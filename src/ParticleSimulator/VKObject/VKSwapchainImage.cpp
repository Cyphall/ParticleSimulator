#include "VKSwapchainImage.h"

#include <ParticleSimulator/RenderContext.h>
#include <ParticleSimulator/VKObject/VKContext.h>

VKSwapchainImage::VKSwapchainImage(vk::Image handle, vk::Format format, vk::Extent2D extent):
	VKImageBase(format, extent, vk::ImageLayout::eUndefined, 1, vk::ImageAspectFlagBits::eColor)
{
	_handle = handle;
	
	vk::ImageViewCreateInfo imageViewCreateInfo;
	imageViewCreateInfo.image = handle;
	imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
	imageViewCreateInfo.format = format;
	imageViewCreateInfo.components.r = vk::ComponentSwizzle::eIdentity;
	imageViewCreateInfo.components.g = vk::ComponentSwizzle::eIdentity;
	imageViewCreateInfo.components.b = vk::ComponentSwizzle::eIdentity;
	imageViewCreateInfo.components.a = vk::ComponentSwizzle::eIdentity;
	imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;
	
	_view = RenderContext::vkContext->getDevice().createImageView(imageViewCreateInfo);
}

VKSwapchainImage::~VKSwapchainImage()
{
	RenderContext::vkContext->getDevice().destroyImageView(_view);
}