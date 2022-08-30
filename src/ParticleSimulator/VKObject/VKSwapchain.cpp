#include "VKSwapchain.h"

#include <ParticleSimulator/RenderContext.h>
#include <ParticleSimulator/VKObject/VKContext.h>
#include <ParticleSimulator/VKObject/VKSwapchainImage.h>
#include <ParticleSimulator/VKObject/VKSwapchainSemaphore.h>

#include <GLFW/glfw3.h>
#include <iostream>

VKSwapchain::VKSwapchain()
{
	createSwapchain();
	createSemaphore();
}

VKSwapchain::~VKSwapchain()
{
	_semaphore.reset();
	RenderContext::vkContext->getDevice().destroySwapchainKHR(_swapchain);
}

size_t VKSwapchain::getDynamicCount() const
{
	return _swapchainImages.size();
}

std::tuple<VKSwapchainImage&, vk::Semaphore> VKSwapchain::retrieveNextImage()
{
	_currentDynamicIndex = (_currentDynamicIndex + 1) % getDynamicCount();
	
	vk::Semaphore currentSemaphore = _semaphore->get();
	
	auto [result, imageIndex] = RenderContext::vkContext->getDevice().acquireNextImageKHR(_swapchain, UINT64_MAX, currentSemaphore, VK_NULL_HANDLE);
	if (result == vk::Result::eSuboptimalKHR)
	{
		std::cout << "WARNING: Suboptimal swapchain." << std::endl;
	}
	_currentImageIndex = imageIndex;
	
	return std::make_tuple(std::ref(*_swapchainImages[_currentImageIndex]), currentSemaphore);
}

uint32_t VKSwapchain::getCurrentDynamicIndex() const
{
	return _currentDynamicIndex;
}

void VKSwapchain::present(vk::Semaphore semaphoreToWait)
{
	vk::PresentInfoKHR presentInfo;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &semaphoreToWait;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &_swapchain;
	presentInfo.pImageIndices = &_currentImageIndex;
	presentInfo.pResults = nullptr; // Optional
	
	RenderContext::vkContext->getMainQueue().presentKHR(presentInfo);
}

vk::Format VKSwapchain::getFormat() const
{
	return _swapchainImages.front()->getFormat();
}

vk::Extent2D VKSwapchain::getExtent() const
{
	return _swapchainImages.front()->getExtent();
}

vk::SurfaceFormatKHR VKSwapchain::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
{
	for (const vk::SurfaceFormatKHR& availableFormat : availableFormats)
	{
		if (availableFormat.format == vk::Format::eB8G8R8A8Unorm && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
		{
			return availableFormat;
		}
	}
	
	std::cerr << "ERROR: Preferred swapchain format not supported." << std::endl;
	return availableFormats.front();
}

VKSwapchain::PresentModeDetails VKSwapchain::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
{
	return {vk::PresentModeKHR::eFifo, 2};
}

vk::Extent2D VKSwapchain::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != UINT32_MAX)
	{
		return capabilities.currentExtent;
	}
	
	int width;
	int height;
	glfwGetFramebufferSize(RenderContext::vkContext->getWindow(), &width, &height);
	
	vk::Extent2D extent;
	extent.width = width;
	extent.height = height;
	
	extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
	extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
	
	return extent;
}

void VKSwapchain::createSwapchain()
{
	VKContext::SwapChainSupportDetails swapChainSupport = RenderContext::vkContext->querySwapchainSupport(RenderContext::vkContext->getPhysicalDevice());
	
	vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	PresentModeDetails presentModeDetails = chooseSwapPresentMode(swapChainSupport.presentModes);
	vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
	
	uint32_t imageCount = std::clamp(
		presentModeDetails.preferredImageCount,
		swapChainSupport.capabilities.minImageCount,
		swapChainSupport.capabilities.maxImageCount == 0 ? std::numeric_limits<uint32_t>::max() : swapChainSupport.capabilities.maxImageCount);
	
	vk::SwapchainCreateInfoKHR createInfo;
	createInfo.surface = RenderContext::vkContext->getSurface();
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eStorage;
	createInfo.imageSharingMode = vk::SharingMode::eExclusive;
	createInfo.queueFamilyIndexCount = 0; // Optional
	createInfo.pQueueFamilyIndices = nullptr; // Optional
	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	createInfo.presentMode = presentModeDetails.presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;
	
	_swapchain = RenderContext::vkContext->getDevice().createSwapchainKHR(createInfo);
	
	std::vector<vk::Image> swapchainImages = RenderContext::vkContext->getDevice().getSwapchainImagesKHR(_swapchain);
	for (vk::Image image : swapchainImages)
	{
		_swapchainImages.push_back(std::make_unique<VKSwapchainImage>(image, surfaceFormat.format, extent));
	}
}

void VKSwapchain::createSemaphore()
{
	vk::SemaphoreCreateInfo semaphoreCreateInfo;
	_semaphore = std::make_unique<VKSwapchainSemaphore>(getDynamicCount(), semaphoreCreateInfo);
}