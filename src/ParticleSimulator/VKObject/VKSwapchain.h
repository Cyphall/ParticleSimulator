#pragma once

#include <ParticleSimulator/VKObject/VKObject.h>

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

template<typename T>
class VKDynamic;
class VKSwapchainImage;
class VKSemaphore;

class VKSwapchain : public VKObject
{
public:
	static std::unique_ptr<VKSwapchain> create(VKContext& context);
	
	~VKSwapchain() override;
	
	size_t getDynamicCount() const;
	
	std::tuple<const VKPtr<VKSwapchainImage>&, const VKPtr<VKSemaphore>&> retrieveNextImage();
	
	uint32_t getCurrentDynamicIndex() const;
	uint32_t getCurrentImageIndex() const;
	
	vk::SwapchainKHR& getHandle();
	
	vk::Format getFormat() const;
	const glm::uvec2& getSize() const;
	
private:
	struct PresentModeDetails
	{
		vk::PresentModeKHR presentMode;
		uint32_t preferredImageCount;
	};
	
	explicit VKSwapchain(VKContext& context);
	
	vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
	PresentModeDetails chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
	vk::Extent2D chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities);
	
	void createSwapchain();
	void createSemaphore();
	
	vk::SwapchainKHR _swapchain;
	
	std::vector<VKPtr<VKSwapchainImage>> _swapchainImages;
	uint32_t _currentImageIndex = -1;
	
	std::unique_ptr<VKDynamic<VKSemaphore>> _semaphore;
	
	uint32_t _currentDynamicIndex = 0;
};