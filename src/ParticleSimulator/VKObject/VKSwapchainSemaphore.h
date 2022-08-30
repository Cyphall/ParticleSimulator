#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>

class VKSwapchainSemaphore
{
public:
	VKSwapchainSemaphore(size_t count, vk::SemaphoreCreateInfo semaphoreCreateInfo);
	~VKSwapchainSemaphore();
	
	vk::Semaphore get();

private:
	std::vector<vk::Semaphore> _semaphores;
};
