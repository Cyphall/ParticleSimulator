#include "VKDynamicImage.h"

#include <ParticleSimulator/RenderContext.h>
#include <ParticleSimulator/VKObject/VKSwapchain.h>
#include <ParticleSimulator/VKObject/VKImage.h>

VKDynamicImage::VKDynamicImage(
	vk::Format format,
	vk::Extent2D extent,
	vk::ImageLayout layout,
	int mipLevels,
	vk::ImageTiling tiling,
	vk::ImageUsageFlags usage,
	vk::ImageAspectFlagBits aspect,
	vk::MemoryPropertyFlags requiredProperties,
	vk::MemoryPropertyFlags preferredProperties)
{
	size_t count = RenderContext::swapchain->getDynamicCount();
	for (size_t i = 0; i < count; i++)
	{
		_images.push_back(std::make_unique<VKImage>(format, extent, layout, mipLevels, tiling, usage, aspect, requiredProperties, preferredProperties));
	}
}

VKDynamicImage::~VKDynamicImage()
{
	
}

VKDynamicImage::operator VKImage&()
{
	return get();
}

VKImage& VKDynamicImage::get()
{
	return *_images[RenderContext::swapchain->getCurrentDynamicIndex()];
}