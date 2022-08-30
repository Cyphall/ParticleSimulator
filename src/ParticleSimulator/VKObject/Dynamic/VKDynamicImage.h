#pragma once

#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

class VKImage;

class VKDynamicImage
{
public:
	VKDynamicImage(
		vk::Format format,
		vk::Extent2D extent,
		vk::ImageLayout layout,
		int mipLevels,
		vk::ImageTiling tiling,
		vk::ImageUsageFlags usage,
		vk::ImageAspectFlagBits aspect,
		vk::MemoryPropertyFlags requiredProperties = {},
		vk::MemoryPropertyFlags preferredProperties = {});
	~VKDynamicImage();
	
	operator VKImage&();
	
	VKImage& get();

private:
	std::vector<std::unique_ptr<VKImage>> _images;
	
};