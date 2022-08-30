#include "VKDynamicCommandBuffer.h"

#include <ParticleSimulator/RenderContext.h>
#include <ParticleSimulator/VKObject/VKContext.h>
#include <ParticleSimulator/VKObject/VKSwapchain.h>

VKDynamicCommandBuffer::VKDynamicCommandBuffer(vk::CommandPool commandPool, vk::CommandBufferLevel level):
_commandPool(commandPool)
{
	vk::CommandBufferAllocateInfo allocateInfo;
	allocateInfo.commandPool = commandPool;
	allocateInfo.level = level;
	allocateInfo.commandBufferCount = RenderContext::swapchain->getDynamicCount();
	
	_commandBuffers = RenderContext::vkContext->getDevice().allocateCommandBuffers(allocateInfo);
}

VKDynamicCommandBuffer::~VKDynamicCommandBuffer()
{
	RenderContext::vkContext->getDevice().freeCommandBuffers(_commandPool, _commandBuffers);
}

vk::CommandBuffer VKDynamicCommandBuffer::get()
{
	return _commandBuffers[RenderContext::swapchain->getCurrentDynamicIndex()];
}