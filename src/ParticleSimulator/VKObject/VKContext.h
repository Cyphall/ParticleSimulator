#pragma once

#include <ParticleSimulator/VKObject/VKPtr.h>

#include <vector>
#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.hpp>
#include <functional>
#include <filesystem>
#include <glm/glm.hpp>

struct GLFWwindow;
class VKQueue;
class VKCommandBuffer;

class VKContext
{
public:
	static std::unique_ptr<VKContext> create(GLFWwindow* window);
	
	struct SwapChainSupportDetails
	{
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> presentModes;
	};
	
	~VKContext();
	
	vk::Instance getInstance();
	vk::SurfaceKHR getSurface();
	vk::PhysicalDevice getPhysicalDevice();
	vk::Device getDevice();
	VKQueue& getQueue();
	
	GLFWwindow* getWindow();
	
	SwapChainSupportDetails querySwapchainSupport(const vk::PhysicalDevice& device) const;
	
	vma::Allocator getVmaAllocator();
	
	void executeImmediate(std::function<void(const VKPtr<VKCommandBuffer>& commandBuffer)>&& function);

private:
	std::vector<const char*> _instanceExtensions;
	std::vector<const char*> _layers;
	std::vector<const char*> _deviceExtensions;
	
	GLFWwindow* _window = nullptr;
	
	vk::Instance _instance;
	
	vk::DebugUtilsMessengerEXT _messenger;
	vk::SurfaceKHR _surface;
	vk::PhysicalDevice _physicalDevice;
	std::unique_ptr<VKQueue> _queue;
	vk::Device _device;
	
	vma::Allocator _vmaAllocator;
	VKPtr<VKCommandBuffer> _immediateCommandBuffer;
	
	explicit VKContext(GLFWwindow* window);
	
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
	uint32_t findSuitableQueueFamily();
	void createLogicalDevice();
	
	void createVmaAllocator();
	void createImmediateCommandBuffer();
};