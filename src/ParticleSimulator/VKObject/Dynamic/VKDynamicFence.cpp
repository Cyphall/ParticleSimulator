#include "VKDynamicFence.h"

#include <ParticleSimulator/RenderContext.h>
#include <ParticleSimulator/VKObject/VKContext.h>
#include <ParticleSimulator/VKObject/VKSwapchain.h>

VKDynamicFence::VKDynamicFence(vk::FenceCreateInfo fenceCreateInfo)
{
	size_t count = RenderContext::swapchain->getDynamicCount();
	
	_fences.reserve(count);
	for (int i = 0; i < count; i++)
	{
		_fences.push_back(RenderContext::vkContext->getDevice().createFence(fenceCreateInfo));
	}
}

VKDynamicFence::~VKDynamicFence()
{
	for (vk::Fence fence : _fences)
	{
		RenderContext::vkContext->getDevice().destroyFence(fence);
	}
}

vk::Fence VKDynamicFence::get()
{
	return _fences[RenderContext::swapchain->getCurrentDynamicIndex()];
}