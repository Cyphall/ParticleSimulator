#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>

class VKDynamicSemaphore
{
public:
	explicit VKDynamicSemaphore(vk::SemaphoreCreateInfo semaphoreCreateInfo);
	~VKDynamicSemaphore();
	
	vk::Semaphore get();

private:
	std::vector<vk::Semaphore> _semaphores;
};
