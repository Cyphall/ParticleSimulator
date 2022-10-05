#include "VKQueue.h"

#include <ParticleSimulator/VKObject/VKContext.h>
#include <ParticleSimulator/VKObject/CommandBuffer/VKCommandBuffer.h>
#include <ParticleSimulator/VKObject/Semaphore/VKSemaphore.h>
#include <ParticleSimulator/VKObject/VKSwapchain.h>
#include <ParticleSimulator/VKObject/Fence/VKFence.h>

VKQueue::VKQueue(VKContext& context, uint32_t queueFamily, uint32_t queueIndex):
	VKObject(context), _queueFamily(queueFamily)
{
	_queue = _context.getDevice().getQueue(queueFamily, queueIndex);
}

vk::Queue& VKQueue::getHandle()
{
	return _queue;
}

uint32_t VKQueue::getFamily() const
{
	return _queueFamily;
}

void VKQueue::submit(const VKPtr<VKCommandBuffer>& commandBuffer, vk::PipelineStageFlags waitStages, const VKPtr<VKSemaphore>* waitSemaphore, const VKPtr<VKSemaphore>* signalSemaphore)
{
	vk::SubmitInfo submitInfo;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer->getHandle();
	submitInfo.pWaitDstStageMask = &waitStages;
	
	if (waitSemaphore != nullptr)
	{
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &waitSemaphore->get()->getHandle();
	}
	else
	{
		submitInfo.waitSemaphoreCount = 0;
		submitInfo.pWaitSemaphores = nullptr;
	}
	
	if (signalSemaphore != nullptr)
	{
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &signalSemaphore->get()->getHandle();
	}
	else
	{
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.pSignalSemaphores = nullptr;
	}
	
	commandBuffer->getStatusFence()->reset();
	_queue.submit(submitInfo, commandBuffer->getStatusFence()->getHandle());
}

void VKQueue::present(VKSwapchain& swapchain, const VKPtr<VKSemaphore>* waitSemaphore)
{
	uint32_t imageIndex = swapchain.getCurrentImageIndex();
	
	vk::PresentInfoKHR presentInfo;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapchain.getHandle();
	presentInfo.pImageIndices = &imageIndex;
	
	if (waitSemaphore != nullptr)
	{
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &waitSemaphore->get()->getHandle();
	}
	else
	{
		presentInfo.waitSemaphoreCount = 0;
		presentInfo.pWaitSemaphores = nullptr;
	}
	
	_queue.presentKHR(presentInfo);
}