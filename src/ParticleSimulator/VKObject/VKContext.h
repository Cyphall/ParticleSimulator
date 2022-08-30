#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.hpp>
#include <functional>

struct GLFWwindow;

class VKContext
{
public:
	struct SwapChainSupportDetails
	{
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> presentModes;
	};
	
	explicit VKContext(GLFWwindow* window);
	~VKContext();
	
	vk::Instance getInstance();
	vk::SurfaceKHR getSurface();
	vk::PhysicalDevice getPhysicalDevice();
	vk::Device getDevice();
	uint32_t getMainQueueFamily();
	vk::Queue getMainQueue();
	
	GLFWwindow* getWindow();
	
	SwapChainSupportDetails querySwapchainSupport(const vk::PhysicalDevice& device) const;
	
	vma::Allocator getVmaAllocator();
	
	void executeImmediate(std::function<void(vk::CommandBuffer)>&& function);

private:
	std::vector<const char*> _instanceExtensions;
	std::vector<const char*> _layers;
	std::vector<const char*> _deviceExtensions;
	
	GLFWwindow* _window = nullptr;
	
	vk::Instance _instance;
	
	vk::DebugUtilsMessengerEXT _messenger;
	vk::SurfaceKHR _surface;
	vk::PhysicalDevice _physicalDevice;
	uint32_t _mainQueueFamily;
	vk::Queue _mainQueue;
	vk::Device _device;
	
	vma::Allocator _vmaAllocator;
	vk::Fence _immediateFence;
	vk::CommandPool _immediateCommandPool;
	vk::CommandBuffer _immediateCommandBuffer;
	
	int calculateDeviceScore(const vk::PhysicalDevice& device) const;
	bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device) const;
	
	void fillInstanceExtensions();
	void fillLayers();
	void fillDeviceExtensions();
	
	void checkInstanceExtensionSupport();
	void checkLayerSupport();
	
	void createInstance();
	
	void createMessenger();
	void createSurface();
	void selectPhysicalDevice();
	void selectQueueFamilies();
	void createLogicalDevice();
	
	void createVmaAllocator();
	void createImmediateFence();
	void createImmediateCommandPool();
	void createImmediateCommandBuffer();
};