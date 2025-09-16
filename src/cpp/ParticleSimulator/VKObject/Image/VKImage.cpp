#include "VKImage.h"

#include <ParticleSimulator/VKObject/VKContext.h>
#include <ParticleSimulator/VKObject/VKDynamic.h>

VKPtr<VKImage> VKImage::create(
	VKContext& context,
	vk::Format format,
	const glm::uvec2& size,
	vk::ImageLayout layout,
	int mipLevels,
	vk::ImageTiling tiling,
	vk::ImageUsageFlags usage,
	vk::ImageAspectFlagBits aspect,
	vk::MemoryPropertyFlags requiredProperties,
	vk::MemoryPropertyFlags preferredProperties)
{
	return VKPtr<VKImage>(new VKImage(
		context,
		format,
		size,
		layout,
		mipLevels,
		tiling,
		usage,
		aspect,
		requiredProperties,
		preferredProperties));
}

std::unique_ptr<VKDynamic<VKImage>> VKImage::createDynamic(
	VKContext& context,
	VKSwapchain& swapchain,
	vk::Format format,
	const glm::uvec2& size,
	vk::ImageLayout layout,
	int mipLevels,
	vk::ImageTiling tiling,
	vk::ImageUsageFlags usage,
	vk::ImageAspectFlagBits aspect,
	vk::MemoryPropertyFlags requiredProperties,
	vk::MemoryPropertyFlags preferredProperties)
{
	return std::unique_ptr<VKDynamic<VKImage>>(new VKDynamic<VKImage>(
		context,
		swapchain,
		format,
		size,
		layout,
		mipLevels,
		tiling,
		usage,
		aspect,
		requiredProperties,
		preferredProperties));
}

VKImage::VKImage(
	VKContext& context,
	vk::Format format,
	const glm::uvec2& size,
	vk::ImageLayout layout,
	int mipLevels,
	vk::ImageTiling tiling,
	vk::ImageUsageFlags usage,
	vk::ImageAspectFlagBits aspect,
	vk::MemoryPropertyFlags requiredProperties,
	vk::MemoryPropertyFlags preferredProperties): VKImageBase(context, format, size, layout, mipLevels, aspect)
{
	vk::ImageCreateInfo createInfo;
	createInfo.imageType = vk::ImageType::e2D;
	createInfo.extent = vk::Extent3D(size.x, size.y, 1);
	createInfo.mipLevels = mipLevels;
	createInfo.arrayLayers = 1;
	createInfo.format = format;
	createInfo.tiling = tiling;
	createInfo.initialLayout = layout;
	createInfo.usage = usage;
	createInfo.sharingMode = vk::SharingMode::eExclusive;
	createInfo.samples = vk::SampleCountFlagBits::e1;
	createInfo.flags = {};

	VmaAllocationCreateInfo allocationCreateInfo{};
	allocationCreateInfo.usage = VMA_MEMORY_USAGE_UNKNOWN;
	allocationCreateInfo.requiredFlags = static_cast<VkMemoryPropertyFlags>(requiredProperties);
	allocationCreateInfo.preferredFlags = static_cast<VkMemoryPropertyFlags>(preferredProperties);

	vmaCreateImage(
		_context.getVmaAllocator(),
		reinterpret_cast<VkImageCreateInfo*>(&createInfo),
		&allocationCreateInfo,
		reinterpret_cast<VkImage*>(&_handle),
		&_imageAlloc,
		nullptr
	);

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

	_view = _context.getDevice().createImageView(imageViewCreateInfo);
}

VKImage::~VKImage()
{
	_context.getDevice().destroyImageView(_view);
	vmaDestroyImage(_context.getVmaAllocator(), _handle, _imageAlloc);
}