#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>

class VKDynamicFence
{
public:
	explicit VKDynamicFence(vk::FenceCreateInfo fenceCreateInfo);
	~VKDynamicFence();
	
	vk::Fence get();

private:
	std::vector<vk::Fence> _fences;
};