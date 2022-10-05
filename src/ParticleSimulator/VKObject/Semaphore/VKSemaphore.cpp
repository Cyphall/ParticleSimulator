#include "VKSemaphore.h"

#include <ParticleSimulator/VKObject/VKContext.h>
#include <ParticleSimulator/VKObject/VKDynamic.h>

VKPtr<VKSemaphore> VKSemaphore::create(VKContext& context, const vk::SemaphoreCreateInfo& semaphoreCreateInfo)
{
	return VKPtr<VKSemaphore>(new VKSemaphore(context, semaphoreCreateInfo));
}

std::unique_ptr<VKDynamic<VKSemaphore>> VKSemaphore::createDynamic(VKContext& context, VKSwapchain& swapchain, const vk::SemaphoreCreateInfo& semaphoreCreateInfo)
{
	return std::unique_ptr<VKDynamic<VKSemaphore>>(new VKDynamic<VKSemaphore>(context, swapchain, semaphoreCreateInfo));
}

VKSemaphore::VKSemaphore(VKContext& context, const vk::SemaphoreCreateInfo& semaphoreCreateInfo):
	VKObject(context)
{
	_semaphore = _context.getDevice().createSemaphore(semaphoreCreateInfo);
}

VKSemaphore::~VKSemaphore()
{
	_context.getDevice().destroySemaphore(_semaphore);
}

vk::Semaphore& VKSemaphore::getHandle()
{
	return _semaphore;
}