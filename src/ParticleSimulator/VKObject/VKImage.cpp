#include "VKImage.h"

#include <ParticleSimulator/RenderContext.h>
#include <ParticleSimulator/VKObject/VKContext.h>

VKImage::VKImage(
	vk::Format format,
	vk::Extent2D extent,
	vk::ImageLayout layout,
	int mipLevels,
	vk::ImageTiling tiling,
	vk::ImageUsageFlags usage,
	vk::ImageAspectFlagBits aspect,
	vk::MemoryPropertyFlags requiredProperties,
	vk::MemoryPropertyFlags preferredProperties):
	VKImageBase(format, extent, layout, mipLevels, aspect)
{
	vk::ImageCreateInfo createInfo;
	createInfo.imageType = vk::ImageType::e2D;
	createInfo.extent = vk::Extent3D(extent, 1);
	createInfo.mipLevels = mipLevels;
	createInfo.arrayLayers = 1;
	createInfo.format = format;
	createInfo.tiling = tiling;
	createInfo.initialLayout = layout;
	createInfo.usage = usage;
	createInfo.sharingMode = vk::SharingMode::eExclusive;
	createInfo.samples = vk::SampleCountFlagBits::e1;
	createInfo.flags = {};
	
	vma::AllocationCreateInfo allocationCreateInfo;
	allocationCreateInfo.usage = vma::MemoryUsage::eUnknown;
	allocationCreateInfo.requiredFlags = requiredProperties;
	allocationCreateInfo.preferredFlags = preferredProperties;
	
	std::tie(_handle, _imageAlloc) = RenderContext::vkContext->getVmaAllocator().createImage(createInfo, allocationCreateInfo);
	
	vk::ImageViewCreateInfo imageViewCreateInfo;
	imageViewCreateInfo.image = _handle;
	imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
	imageViewCreateInfo.format = format;
	imageViewCreateInfo.components.r = vk::ComponentSwizzle::eIdentity;
	imageViewCreateInfo.components.g = vk::ComponentSwizzle::eIdentity;
	imageViewCreateInfo.components.b = vk::ComponentSwizzle::eIdentity;
	imageViewCreateInfo.components.a = vk::ComponentSwizzle::eIdentity;
	imageViewCreateInfo.subresourceRange.aspectMask = aspect;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = mipLevels;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;
	
	_view = RenderContext::vkContext->getDevice().createImageView(imageViewCreateInfo);
}

VKImage::~VKImage()
{
	RenderContext::vkContext->getDevice().destroyImageView(_view);
	RenderContext::vkContext->getVmaAllocator().destroyImage(_handle, _imageAlloc);
}