#include "VKContext.h"

#include <vector>
#include <unordered_set>
#include <map>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <format>
#include <GLFW/glfw3.h>

static const uint32_t VULKAN_VERSION = VK_API_VERSION_1_3;

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

static std::ofstream LOG_FILE = std::ofstream("ParticleSimulator.log", std::ios::out | std::ios::binary);

static VKAPI_ATTR vk::Bool32 VKAPI_CALL messageCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity, vk::DebugUtilsMessageTypeFlagsEXT messageTypes, const vk::DebugUtilsMessengerCallbackDataEXT* messageData, void* userData)
{
	switch (messageSeverity)
	{
		case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
			std::cout << std::format("ERROR: {}", messageData->pMessage) << '\n' << std::endl;
			LOG_FILE << std::format("ERROR: {}", messageData->pMessage) << '\n' << std::endl;
			__debugbreak();
			break;
		case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
			std::cout << std::format("WARNING: {}", messageData->pMessage) << '\n' << std::endl;
			LOG_FILE << std::format("WARNING: {}", messageData->pMessage) << '\n' << std::endl;
			break;
#if _DEBUG
		case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
			std::cout << std::format("INFO: {}", messageData->pMessage) << '\n' << std::endl;
			LOG_FILE << std::format("INFO: {}", messageData->pMessage) << '\n' << std::endl;
			break;
		case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
			std::cout << std::format("NOTIF: {}", messageData->pMessage) << '\n' << std::endl;
			LOG_FILE << std::format("NOTIF: {}", messageData->pMessage) << '\n' << std::endl;
			break;
#endif
	}
	
	return VK_FALSE;
}

VKContext::VKContext(GLFWwindow* window):
_window(window)
{
	VULKAN_HPP_DEFAULT_DISPATCHER.init(vk::DynamicLoader().getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr"));
	
	fillInstanceExtensions();
	fillLayers();
	fillDeviceExtensions();
	
	checkInstanceExtensionSupport();
	checkLayerSupport();
	
	createInstance();
	
	VULKAN_HPP_DEFAULT_DISPATCHER.init(_instance);
	
	createMessenger();
	createSurface();
	selectPhysicalDevice();
	selectQueueFamilies();
	createLogicalDevice();
	
	VULKAN_HPP_DEFAULT_DISPATCHER.init(_device);
	
	createVmaAllocator();
	createImmediateFence();
	createImmediateCommandPool();
	createImmediateCommandBuffer();
}

VKContext::~VKContext()
{
	_device.destroyCommandPool(_immediateCommandPool);
	_device.destroyFence(_immediateFence);
	_vmaAllocator.destroy();
	_device.destroy();
	_instance.destroySurfaceKHR(_surface);
	_instance.destroyDebugUtilsMessengerEXT(_messenger);
	_instance.destroy();
}

vk::Instance VKContext::getInstance()
{
	return _instance;
}

vk::SurfaceKHR VKContext::getSurface()
{
	return _surface;
}

vk::PhysicalDevice VKContext::getPhysicalDevice()
{
	return _physicalDevice;
}

vk::Device VKContext::getDevice()
{
	return _device;
}

uint32_t VKContext::getMainQueueFamily()
{
	return _mainQueueFamily;
}

vk::Queue VKContext::getMainQueue()
{
	return _mainQueue;
}

GLFWwindow* VKContext::getWindow()
{
	return _window;
}

VKContext::SwapChainSupportDetails VKContext::querySwapchainSupport(const vk::PhysicalDevice& device) const
{
	SwapChainSupportDetails details;
	
	details.capabilities = device.getSurfaceCapabilitiesKHR(_surface);
	details.formats = device.getSurfaceFormatsKHR(_surface);
	details.presentModes = device.getSurfacePresentModesKHR(_surface);
	
	return details;
}

int VKContext::calculateDeviceScore(const vk::PhysicalDevice& device) const
{
	vk::PhysicalDeviceProperties deviceProperties = device.getProperties();
	
	if (deviceProperties.apiVersion < VULKAN_VERSION)
	{
		return 0;
	}
	
	if (!checkDeviceExtensionSupport(device))
	{
		return 0;
	}
	
	SwapChainSupportDetails swapChainSupport = querySwapchainSupport(device);
	if (swapChainSupport.formats.empty() || swapChainSupport.presentModes.empty())
	{
		return 0;
	}
	
	// from here, device is at least compatible
	
	int score = 1;
	
	if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
	{
		score += 1000;
	}
	else if (deviceProperties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu)
	{
		score += 100;
	}
	
	return score;
}

bool VKContext::checkDeviceExtensionSupport(const vk::PhysicalDevice& device) const
{
	std::unordered_set<std::string> supportedDeviceExtensions;
	
	for (const vk::ExtensionProperties& deviceExtension : device.enumerateDeviceExtensionProperties())
	{
		supportedDeviceExtensions.insert(deviceExtension.extensionName);
	}
	
	for (const std::string& layer : _layers)
	{
		for (const vk::ExtensionProperties& layerDeviceExtension : device.enumerateDeviceExtensionProperties(layer))
		{
			supportedDeviceExtensions.insert(layerDeviceExtension.extensionName);
		}
	}
	
	for (const char* wantedDeviceExtension : _deviceExtensions)
	{
		bool found = false;
		for (const std::string& supportedDeviceExtension : supportedDeviceExtensions)
		{
			if (wantedDeviceExtension == supportedDeviceExtension)
			{
				found = true;
				break;
			}
		}
		
		if (!found)
		{
			return false;
		}
	}
	
	return true;
}

void VKContext::fillInstanceExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	
	for (int i = 0; i < glfwExtensionCount; i++)
	{
		_instanceExtensions.push_back(glfwExtensions[i]);
	}

	_instanceExtensions.push_back("VK_EXT_debug_utils");
#if _DEBUG
	_instanceExtensions.push_back("VK_EXT_validation_features");
#endif
}

void VKContext::fillLayers()
{
#if _DEBUG
	_layers.push_back("VK_LAYER_KHRONOS_validation");
#endif
}

void VKContext::fillDeviceExtensions()
{
	_deviceExtensions.push_back("VK_KHR_swapchain");
}

void VKContext::checkInstanceExtensionSupport()
{
	std::unordered_set<std::string> supportedInstanceExtensions;
	
	for (const vk::ExtensionProperties& instanceExtension : vk::enumerateInstanceExtensionProperties())
	{
		supportedInstanceExtensions.insert(instanceExtension.extensionName);
	}
	
	for (const std::string& layer : _layers)
	{
		for (const vk::ExtensionProperties& layerInstanceExtension : vk::enumerateInstanceExtensionProperties(layer))
		{
			supportedInstanceExtensions.insert(layerInstanceExtension.extensionName);
		}
	}
	
	for (const char* wantedInstanceExtension : _instanceExtensions)
	{
		bool found = false;
		for (const std::string& supportedInstanceExtension : supportedInstanceExtensions)
		{
			if (wantedInstanceExtension == supportedInstanceExtension)
			{
				found = true;
				break;
			}
		}
		
		if (!found)
		{
			std::stringstream errorMessage;
			errorMessage << "Vulkan instance extension \"" << wantedInstanceExtension << "\" is not supported by this driver.\n";
			errorMessage << "Please make sure your GPU is compatible and your driver is up to date.\n\n";
			throw std::runtime_error(errorMessage.str());
		}
	}
}

void VKContext::checkLayerSupport()
{
	std::unordered_set<std::string> supportedLayers;
	
	for (const vk::LayerProperties& layer : vk::enumerateInstanceLayerProperties())
	{
		supportedLayers.insert(layer.layerName);
	}
	
	for (const char* wantedLayers : _layers)
	{
		bool found = false;
		for (const std::string& supportedLayer : supportedLayers)
		{
			if (wantedLayers == supportedLayer)
			{
				found = true;
				break;
			}
		}
		
		if (!found)
		{
			std::stringstream errorMessage;
			errorMessage << "Vulkan layer \"" << wantedLayers << "\" is not supported by this driver.\n";
			errorMessage << "Please make sure your GPU is compatible and your driver is up to date.\n\n";
			throw std::runtime_error(errorMessage.str());
		}
	}
}

void VKContext::createInstance()
{
	vk::ApplicationInfo appInfo;
	appInfo.pApplicationName = "Meinkraft";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VULKAN_VERSION;
	
	vk::InstanceCreateInfo createInfo;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = _instanceExtensions.size();
	createInfo.ppEnabledExtensionNames = _instanceExtensions.data();
	createInfo.enabledLayerCount = _layers.size();
	createInfo.ppEnabledLayerNames = _layers.data();

#if _DEBUG
	std::vector<vk::ValidationFeatureEnableEXT> enabledFeatures{
		vk::ValidationFeatureEnableEXT::eGpuAssisted,
		vk::ValidationFeatureEnableEXT::eBestPractices,
		vk::ValidationFeatureEnableEXT::eSynchronizationValidation
	};
	
	vk::ValidationFeaturesEXT validationFeatures;
	validationFeatures.enabledValidationFeatureCount = enabledFeatures.size();
	validationFeatures.pEnabledValidationFeatures = enabledFeatures.data();
	
	createInfo.pNext = &validationFeatures;
#endif
	
	_instance = vk::createInstance(createInfo);
}

void VKContext::createMessenger()
{
	vk::DebugUtilsMessengerCreateInfoEXT createInfo;
	createInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
	createInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
	createInfo.pfnUserCallback = reinterpret_cast<PFN_vkDebugUtilsMessengerCallbackEXT>(messageCallback);
	createInfo.pUserData = nullptr; // Optional
	
	_messenger = _instance.createDebugUtilsMessengerEXT(createInfo);
}

void VKContext::createSurface()
{
	VkSurfaceKHR surface;
	glfwCreateWindowSurface(_instance, _window, nullptr, &surface);
	_surface = surface;
}

void VKContext::selectPhysicalDevice()
{
	std::vector<vk::PhysicalDevice> devices = _instance.enumeratePhysicalDevices();
	
	if (devices.empty())
	{
		std::stringstream errorMessage;
		errorMessage << "Could not find a device supporting Vulkan.\n";
		errorMessage << "Please make sure your GPU is compatible and your driver is up to date.\n\n";
		throw std::runtime_error(errorMessage.str());
	}
	
	vk::PhysicalDevice bestDevice;
	int bestDeviceScore = 0;
	for (const vk::PhysicalDevice& device : devices)
	{
		int deviceScore = calculateDeviceScore(device);
		if (deviceScore > bestDeviceScore)
		{
			bestDevice = device;
			bestDeviceScore = deviceScore;
		}
	}
	
	if (!bestDevice)
	{
		std::stringstream errorMessage;
		errorMessage << "Could not find a device supporting Vulkan 1.2.\n";
		errorMessage << "Please make sure your GPU is compatible and your driver is up to date.\n\n";
		throw std::runtime_error(errorMessage.str());
	}
	
	_physicalDevice = bestDevice;
}

void VKContext::selectQueueFamilies()
{
	std::vector<vk::QueueFamilyProperties> vkQueueFamilies = _physicalDevice.getQueueFamilyProperties();
	
	vk::QueueFlags flags = vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eTransfer;
	
	for (int i = 0; i < vkQueueFamilies.size(); i++)
	{
		vk::QueueFamilyProperties queueFamilyProperties = vkQueueFamilies[i];
		if ((queueFamilyProperties.queueFlags & flags) == flags && _physicalDevice.getSurfaceSupportKHR(i, _surface))
		{
			_mainQueueFamily = i;
			break;
		}
	}
}

void VKContext::createLogicalDevice()
{
	std::map<uint32_t, size_t> queueFamilies;
	queueFamilies[_mainQueueFamily]++;
	
	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
	std::vector<std::vector<float>> queuesPriorities;
	for (const auto& [id, count] : queueFamilies)
	{
		std::vector<float>& queuePriorities = queuesPriorities.emplace_back(count);
		for (int i = 0; i < count; i++)
		{
			queuePriorities[i] = 1.0f;
		}
		
		vk::DeviceQueueCreateInfo& queueCreateInfo = queueCreateInfos.emplace_back();
		queueCreateInfo.queueFamilyIndex = id;
		queueCreateInfo.queueCount = count;
		queueCreateInfo.pQueuePriorities = queuePriorities.data();
	}
	
	vk::PhysicalDeviceRobustness2FeaturesEXT robustness2Features;
	robustness2Features.nullDescriptor = VK_TRUE;
	
	vk::PhysicalDeviceMaintenance4Features maintenance4Features;
	maintenance4Features.maintenance4 = VK_TRUE;
	maintenance4Features.pNext = &robustness2Features;
	
	vk::PhysicalDeviceSynchronization2Features synchronization2Features;
	synchronization2Features.synchronization2 = VK_TRUE;
	synchronization2Features.pNext = &maintenance4Features;
	
	vk::PhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures;
	dynamicRenderingFeatures.pNext = &synchronization2Features;
	dynamicRenderingFeatures.dynamicRendering = VK_TRUE;
	
	vk::PhysicalDeviceFeatures2 physicalDeviceFeatures;
	physicalDeviceFeatures.pNext = &dynamicRenderingFeatures;
	physicalDeviceFeatures.features.samplerAnisotropy = true;
	
	vk::DeviceCreateInfo deviceCreateInfo;
	deviceCreateInfo.pNext = &physicalDeviceFeatures;
	deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size();
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.enabledExtensionCount = _deviceExtensions.size();
	deviceCreateInfo.ppEnabledExtensionNames = _deviceExtensions.data();
	deviceCreateInfo.enabledLayerCount = 0;
	deviceCreateInfo.ppEnabledLayerNames = nullptr;
	
	_device = _physicalDevice.createDevice(deviceCreateInfo);
	
	_mainQueue = _device.getQueue(_mainQueueFamily, --queueFamilies[_mainQueueFamily]);
}

vma::Allocator VKContext::getVmaAllocator()
{
	return _vmaAllocator;
}

void VKContext::executeImmediate(std::function<void(vk::CommandBuffer)>&& function)
{
	vk::CommandBufferBeginInfo commandBufferBeginInfo;
	commandBufferBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
	commandBufferBeginInfo.pInheritanceInfo = nullptr; // Optional
	
	_immediateCommandBuffer.begin(commandBufferBeginInfo);
	
	function(_immediateCommandBuffer);
	
	_immediateCommandBuffer.end();
	
	vk::PipelineStageFlags waitStages = vk::PipelineStageFlagBits::eBottomOfPipe;
	
	vk::SubmitInfo submitInfo;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = nullptr;
	submitInfo.pWaitDstStageMask = &waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &_immediateCommandBuffer;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = nullptr;
	
	_mainQueue.submit(submitInfo, _immediateFence);
	
	if (_device.waitForFences(_immediateFence, true, UINT64_MAX) != vk::Result::eSuccess)
	{
		throw;
	}
	_device.resetFences(_immediateFence);
	
	_device.resetCommandPool(_immediateCommandPool);
}

void VKContext::createVmaAllocator()
{
	vma::VulkanFunctions vulkanFunctions{};
	vulkanFunctions.vkGetInstanceProcAddr = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetInstanceProcAddr;
	vulkanFunctions.vkGetDeviceProcAddr = VULKAN_HPP_DEFAULT_DISPATCHER.vkGetDeviceProcAddr;
	
	vma::AllocatorCreateInfo allocatorInfo{};
	allocatorInfo.vulkanApiVersion = VULKAN_VERSION;
	allocatorInfo.instance = _instance;
	allocatorInfo.physicalDevice = _physicalDevice;
	allocatorInfo.device = _device;
	allocatorInfo.pVulkanFunctions = &vulkanFunctions;
	
	vma::createAllocator(&allocatorInfo, &_vmaAllocator);
}

void VKContext::createImmediateFence()
{
	vk::FenceCreateInfo createInfo;
	_immediateFence = _device.createFence(createInfo);
}

void VKContext::createImmediateCommandPool()
{
	vk::CommandPoolCreateInfo poolCreateInfo;
	poolCreateInfo.flags = {};
	poolCreateInfo.queueFamilyIndex = _mainQueueFamily;
	
	_immediateCommandPool = _device.createCommandPool(poolCreateInfo);
}

void VKContext::createImmediateCommandBuffer()
{
	vk::CommandBufferAllocateInfo allocateInfo;
	allocateInfo.commandPool = _immediateCommandPool;
	allocateInfo.level = vk::CommandBufferLevel::ePrimary;
	allocateInfo.commandBufferCount = 1;
	
	_immediateCommandBuffer = _device.allocateCommandBuffers(allocateInfo).front();
}