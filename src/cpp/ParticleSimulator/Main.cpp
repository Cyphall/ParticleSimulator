#include <ParticleSimulator/Camera.h>
#include <ParticleSimulator/Rendering/Renderer.h>

#include <CyphGPU/Context.hpp>
#include <CyphGPU/ContextSession.hpp>
#include <CyphGPU/Device.hpp>
#include <CyphGPU/DeviceSession.hpp>
#include <CyphGPU/Surface.hpp>
#include <CyphGPU/Swapchain.hpp>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

CGPU_DECLARE_SHADER_BUNDLE(shaders)

static std::optional<cgpu::DeviceSessionPtr> createDeviceSession()
{
	cgpu::ContextPtr context = cgpu::Context::create({
		.shader_bundles = {&shaders},
	});

	cgpu::ContextSessionPtr contextSession = cgpu::ContextSession::create(
		context,
		{
			.application_name = "ParticleSimulator",
		}
	);

	std::optional<cgpu::DevicePtr> selectedDevice;
	for (const cgpu::DevicePtr& device : contextSession->getDevices())
	{
		if (device->getCapabilities() & cgpu::Device::Capability::eCore &&
		    device->getCapabilities() & cgpu::Device::Capability::eSwapchain)
		{
			selectedDevice = device;
			break;
		}
	}

	if (!selectedDevice)
	{
		spdlog::error("Could not find a compatible device.");
		return std::nullopt;
	}

	// Create device session
	return cgpu::DeviceSession::create(
		*selectedDevice,
		{}
	);
}

static std::pair<GLFWwindow*, cgpu::SurfacePtr> createWindow(const cgpu::ContextSessionPtr& contextSession)
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

#if _DEBUG
	GLFWwindow* window = glfwCreateWindow(800, 600, "ParticleSimulator", nullptr, nullptr);
#else
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

	GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "ParticleSimulator", monitor, nullptr);
#endif

	// Create surface
	VkSurfaceKHR surface_raw{};
	glfwCreateWindowSurface(contextSession->getHandle(), window, nullptr, &surface_raw);

	cgpu::SurfacePtr surface = cgpu::Surface::create(
		contextSession,
		{
			.surface = surface_raw,
		}
	);

	return {window, surface};
}

int main()
{
	glfwInit();

	glfwSetErrorCallback([](int code, const char* message) { spdlog::error(message); });

	std::optional<cgpu::DeviceSessionPtr> deviceSession = createDeviceSession();
	if (!deviceSession)
	{
		return 1;
	}

	auto [window, surface] = createWindow((*deviceSession)->getDevice()->getContextSession());

	Camera camera({0, 0, 0});

	glm::ivec2 extent;
	glfwGetFramebufferSize(window, &extent.x, &extent.y);

	cgpu::SwapchainPtr swapchain = cgpu::Swapchain::create(
		*deviceSession,
		surface,
		{
			.format = {vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear},
			.preferred_extent = extent,
			.usages = vk::ImageUsageFlagBits::eColorAttachment |
	                  vk::ImageUsageFlagBits::eStorage,
		}
	);

	camera.setScreenSize(swapchain->getExtent());

	Renderer renderer{*deviceSession, window, swapchain->getDesc().format.format};

	double previousTime = 0;
	while (!glfwWindowShouldClose(window))
	{
		double currentTime = glfwGetTime();
		double deltaTime = std::min(currentTime - previousTime, 0.1);
		previousTime = currentTime;

		glfwPollEvents();

		while (!swapchain->tryGetImage())
		{
			glfwGetFramebufferSize(window, &extent.x, &extent.y);
			if (extent.x == 0 || extent.y == 0)
			{
				glfwWaitEvents();
				continue;
			}

			swapchain = cgpu::Swapchain::create(
				*deviceSession,
				surface,
				{
					.format = {vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear},
					.preferred_extent = extent,
					.usages = vk::ImageUsageFlagBits::eColorAttachment |
			                  vk::ImageUsageFlagBits::eStorage,
					.old_swapchain = swapchain,
				}
			);

			camera.setScreenSize(swapchain->getExtent());
		}

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, true);
		}

		camera.update(deltaTime);

		renderer.draw(camera, deltaTime, *swapchain->tryGetImage());

		swapchain->presentImage();
	}

	(*deviceSession)->waitIdle();

	glfwDestroyWindow(window);

	glfwTerminate();

	return 0;
}
