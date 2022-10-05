#pragma once

#include <ParticleSimulator/VKObject/Image/VKImageBase.h>

#include <vk_mem_alloc.hpp>

template<typename T>
class VKDynamic;
class VKSwapchain;

class VKImage : public VKImageBase
{
public:
	static VKPtr<VKImage> create(
		VKContext& context,
		vk::Format format,
		const glm::uvec2& size,
		vk::ImageLayout layout,
		int mipLevels,
		vk::ImageTiling tiling,
		vk::ImageUsageFlags usage,
		vk::ImageAspectFlagBits aspect,
		vk::MemoryPropertyFlags requiredProperties = {},
		vk::MemoryPropertyFlags preferredProperties = {});
	static std::unique_ptr<VKDynamic<VKImage>> createDynamic(
		VKContext& context,
		VKSwapchain& swapchain,
		vk::Format format,
		const glm::uvec2& size,
		vk::ImageLayout layout,
		int mipLevels,
		vk::ImageTiling tiling,
		vk::ImageUsageFlags usage,
		vk::ImageAspectFlagBits aspect,
		vk::MemoryPropertyFlags requiredProperties = {},
		vk::MemoryPropertyFlags preferredProperties = {});
	
	~VKImage() override;
	
private:
	VKImage(
		VKContext& context,
		vk::Format format,
		const glm::uvec2& size,
		vk::ImageLayout layout,
		int mipLevels,
		vk::ImageTiling tiling,
		vk::ImageUsageFlags usage,
		vk::ImageAspectFlagBits aspect,
		vk::MemoryPropertyFlags requiredProperties,
		vk::MemoryPropertyFlags preferredProperties);
	
	vma::Allocation _imageAlloc;
};