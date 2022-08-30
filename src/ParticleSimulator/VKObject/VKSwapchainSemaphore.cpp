#include "VKSwapchainSemaphore.h"

#include <ParticleSimulator/RenderContext.h>
#include <ParticleSimulator/VKObject/VKContext.h>
#include <ParticleSimulator/VKObject/VKSwapchain.h>

VKSwapchainSemaphore::VKSwapchainSemaphore(size_t count, vk::SemaphoreCreateInfo semaphoreCreateInfo)
{
	_semaphores.reserve(count);
	for (int i = 0; i < count; i++)
	{
		_semaphores.push_back(RenderContext::vkContext->getDevice().createSemaphore(semaphoreCreateInfo));
	}
}

VKSwapchainSemaphore::~VKSwapchainSemaphore()
{
	for (vk::Semaphore semaphore : _semaphores)
	{
		RenderContext::vkContext->getDevice().destroySemaphore(semaphore);
	}
}

vk::Semaphore VKSwapchainSemaphore::get()
{
	return _semaphores[RenderContext::swapchain->getCurrentDynamicIndex()];
}