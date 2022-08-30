#pragma once

#include <ParticleSimulator/VKObject/VKImageBase.h>

#include <vk_mem_alloc.hpp>

class VKImage : public VKImageBase
{
public:
	VKImage(
		vk::Format format,
		vk::Extent2D extent,
		vk::ImageLayout layout,
		int mipLevels,
		vk::ImageTiling tiling,
		vk::ImageUsageFlags usage,
		vk::ImageAspectFlagBits aspect,
		vk::MemoryPropertyFlags requiredProperties = {},
		vk::MemoryPropertyFlags preferredProperties = {});
	~VKImage() override;
	
private:
	vma::Allocation _imageAlloc;
};