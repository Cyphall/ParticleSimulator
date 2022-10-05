#pragma once

#include <ParticleSimulator/VKObject/VKObject.h>

#include <vulkan/vulkan.hpp>

template<typename T>
class VKDynamic;
class VKSwapchain;

class VKSemaphore : public VKObject
{
public:
	static VKPtr<VKSemaphore> create(VKContext& context, const vk::SemaphoreCreateInfo& semaphoreCreateInfo);
	static std::unique_ptr<VKDynamic<VKSemaphore>> createDynamic(VKContext& context, VKSwapchain& swapchain, const vk::SemaphoreCreateInfo& semaphoreCreateInfo);
	
	~VKSemaphore() override;
	
	vk::Semaphore& getHandle();

private:
	VKSemaphore(VKContext& context, const vk::SemaphoreCreateInfo& semaphoreCreateInfo);
	
	vk::Semaphore _semaphore;
};