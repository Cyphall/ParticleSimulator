#pragma once

#include <ParticleSimulator/VKObject/VKBuffer.h>
#include <ParticleSimulator/VKObject/VKSwapchain.h>

#include <vulkan/vulkan.hpp>
#include <vector>

template<typename T>
class VKDynamicBuffer
{
public:
	explicit VKDynamicBuffer(vk::BufferUsageFlags pipelineUsage, vk::MemoryPropertyFlags requiredProperties = {}, vk::MemoryPropertyFlags preferredProperties = {})
	{
		size_t count = RenderContext::swapchain->getDynamicCount();
		
		_buffers.reserve(count);
		
		for (int i = 0; i < count; i++)
		{
			_buffers.push_back(std::make_unique<VKBuffer<T>>(pipelineUsage, requiredProperties, preferredProperties));
		}
	}
	
	VKBuffer<T>& get()
	{
		return *_buffers[RenderContext::swapchain->getCurrentDynamicIndex()];
	}

private:
	std::vector<std::unique_ptr<VKBuffer<T>>> _buffers;
};