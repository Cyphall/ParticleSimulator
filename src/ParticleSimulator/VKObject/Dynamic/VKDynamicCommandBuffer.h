#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>

class VKDynamicCommandBuffer
{
public:
	VKDynamicCommandBuffer(vk::CommandPool commandPool, vk::CommandBufferLevel level);
	~VKDynamicCommandBuffer();
	
	vk::CommandBuffer get();
	
private:
	vk::CommandPool _commandPool;
	std::vector<vk::CommandBuffer> _commandBuffers;
};