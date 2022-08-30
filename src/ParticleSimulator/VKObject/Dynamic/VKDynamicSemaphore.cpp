#include "VKDynamicSemaphore.h"

#include <ParticleSimulator/RenderContext.h>
#include <ParticleSimulator/VKObject/VKContext.h>
#include <ParticleSimulator/VKObject/VKSwapchain.h>

VKDynamicSemaphore::VKDynamicSemaphore(vk::SemaphoreCreateInfo semaphoreCreateInfo)
{
	size_t count = RenderContext::swapchain->getDynamicCount();
	
	_semaphores.reserve(count);
	for (int i = 0; i < count; i++)
	{
		_semaphores.push_back(RenderContext::vkContext->getDevice().createSemaphore(semaphoreCreateInfo));
	}
}

VKDynamicSemaphore::~VKDynamicSemaphore()
{
	for (vk::Semaphore semaphore : _semaphores)
	{
		RenderContext::vkContext->getDevice().destroySemaphore(semaphore);
	}
}

vk::Semaphore VKDynamicSemaphore::get()
{
	return _semaphores[RenderContext::swapchain->getCurrentDynamicIndex()];
}