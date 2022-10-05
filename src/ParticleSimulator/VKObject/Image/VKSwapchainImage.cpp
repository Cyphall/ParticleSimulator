#include "VKSwapchainImage.h"

#include <ParticleSimulator/VKObject/VKContext.h>

VKSwapchainImage::VKSwapchainImage(VKContext& context, vk::Image handle, vk::Format format, const glm::uvec2& size):
	VKImageBase(context, format, size, vk::ImageLayout::eUndefined, 1, vk::ImageAspectFlagBits::eColor)
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
	
	_view = _context.getDevice().createImageView(imageViewCreateInfo);
}

VKSwapchainImage::~VKSwapchainImage()
{
	_context.getDevice().destroyImageView(_view);
}