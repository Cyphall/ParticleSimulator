#pragma once

#include <vulkan/vulkan.hpp>

class VKSwapchainImage;
class VKSwapchainSemaphore;

class VKSwapchain
{
public:
	VKSwapchain();
	~VKSwapchain();
	
	size_t getDynamicCount() const;
	
	std::tuple<VKSwapchainImage&, vk::Semaphore> retrieveNextImage();
	
	uint32_t getCurrentDynamicIndex() const;
	
	void present(vk::Semaphore semaphoreToWait);
	
	vk::Format getFormat() const;
	vk::Extent2D getExtent() const;
	
private:
	struct PresentModeDetails
	{
		vk::PresentModeKHR presentMode;
		uint32_t preferredImageCount;
	};
	
	vk::SwapchainKHR _swapchain;
	
	std::vector<std::unique_ptr<VKSwapchainImage>> _swapchainImages;
	uint32_t _currentImageIndex = -1;
	
	std::unique_ptr<VKSwapchainSemaphore> _semaphore;
	
	uint32_t _currentDynamicIndex = 0;
	
	static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
	static PresentModeDetails chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
	static vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);
	
	void createSwapchain();
	void createSemaphore();
};