#include "VKSwapchain.h"

#include <ParticleSimulator/VKObject/VKContext.h>
#include <ParticleSimulator/VKObject/VKDynamic.h>
#include <ParticleSimulator/VKObject/Image/VKSwapchainImage.h>
#include <ParticleSimulator/VKObject/Semaphore/VKSemaphore.h>

#include <GLFW/glfw3.h>
#include <iostream>

std::unique_ptr<VKSwapchain> VKSwapchain::create(VKContext& context)
{
	return std::unique_ptr<VKSwapchain>(new VKSwapchain(context));
}

VKSwapchain::VKSwapchain(VKContext& context):
	VKObject(context)
{
	createSwapchain();
	createSemaphore();
}

VKSwapchain::~VKSwapchain()
{
	_context.getDevice().destroySwapchainKHR(_swapchain);
}

size_t VKSwapchain::getDynamicCount() const
{
	return _swapchainImages.size();
}

std::tuple<const VKPtr<VKSwapchainImage>&, const VKPtr<VKSemaphore>&> VKSwapchain::retrieveNextImage()
{
	_currentDynamicIndex = (_currentDynamicIndex + 1) % getDynamicCount();
	
	const VKPtr<VKSemaphore>& currentSemaphore = *_semaphore;
	
	auto [result, imageIndex] = _context.getDevice().acquireNextImageKHR(_swapchain, UINT64_MAX, currentSemaphore->getHandle(), VK_NULL_HANDLE);
	if (result == vk::Result::eSuboptimalKHR)
	{
		std::cout << "WARNING: Suboptimal swapchain." << std::endl;
	}
	_currentImageIndex = imageIndex;
	
	return std::make_tuple(std::ref(_swapchainImages[_currentImageIndex]), std::ref(currentSemaphore));
}

uint32_t VKSwapchain::getCurrentDynamicIndex() const
{
	return _currentDynamicIndex;
}

uint32_t VKSwapchain::getCurrentImageIndex() const
{
	return _currentImageIndex;
}

vk::SwapchainKHR& VKSwapchain::getHandle()
{
	return _swapchain;
}

vk::Format VKSwapchain::getFormat() const
{
	return _swapchainImages.front()->getFormat();
}

const glm::uvec2& VKSwapchain::getSize() const
{
	return _swapchainImages.front()->getSize();
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
	glfwGetFramebufferSize(_context.getWindow(), &width, &height);
	
	vk::Extent2D extent;
	extent.width = width;
	extent.height = height;
	
	extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
	extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
	
	return extent;
}

void VKSwapchain::createSwapchain()
{
	VKContext::SwapChainSupportDetails swapChainSupport = _context.querySwapchainSupport(_context.getPhysicalDevice());
	
	vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	PresentModeDetails presentModeDetails = chooseSwapPresentMode(swapChainSupport.presentModes);
	vk::Extent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
	
	uint32_t imageCount = std::clamp(
		presentModeDetails.preferredImageCount,
		swapChainSupport.capabilities.minImageCount,
		swapChainSupport.capabilities.maxImageCount == 0 ? std::numeric_limits<uint32_t>::max() : swapChainSupport.capabilities.maxImageCount);
	
	vk::SwapchainCreateInfoKHR createInfo;
	createInfo.surface = _context.getSurface();
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
	
	_swapchain = _context.getDevice().createSwapchainKHR(createInfo);
	
	std::vector<vk::Image> swapchainImages = _context.getDevice().getSwapchainImagesKHR(_swapchain);
	for (vk::Image image : swapchainImages)
	{
		_swapchainImages.push_back(VKPtr<VKSwapchainImage>(new VKSwapchainImage(_context, image, surfaceFormat.format, glm::uvec2(extent.width, extent.height))));
	}
}

void VKSwapchain::createSemaphore()
{
	vk::SemaphoreCreateInfo semaphoreCreateInfo;
	_semaphore = VKSemaphore::createDynamic(_context, *this, semaphoreCreateInfo);
}