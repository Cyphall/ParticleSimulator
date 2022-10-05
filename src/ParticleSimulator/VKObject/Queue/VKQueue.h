#pragma once

#include <ParticleSimulator/VKObject/VKObject.h>

#include <vulkan/vulkan.hpp>

class VKFence;
class VKCommandBuffer;
class VKSemaphore;
class VKSwapchain;

class VKQueue : public VKObject
{
public:
	vk::Queue& getHandle();
	
	uint32_t getFamily() const;
	
	void submit(const VKPtr<VKCommandBuffer>& commandBuffer, vk::PipelineStageFlags waitStages, const VKPtr<VKSemaphore>* waitSemaphore, const VKPtr<VKSemaphore>* signalSemaphore);
	void present(VKSwapchain& swapchain, const VKPtr<VKSemaphore>* waitSemaphore);

private:
	friend class VKContext;
	
	VKQueue(VKContext& context, uint32_t queueFamily, uint32_t queueIndex);
	
	uint32_t _queueFamily;
	vk::Queue _queue;
};