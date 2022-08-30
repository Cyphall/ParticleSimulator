#include <ParticleSimulator/VKObject/VKContext.h>
#include <ParticleSimulator/VKObject/VKSwapchain.h>
#include <ParticleSimulator/Rendering/Renderer.h>
#include <ParticleSimulator/RenderContext.h>
#include <ParticleSimulator/Camera.h>

#include <GLFW/glfw3.h>
#include <iostream>

static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	RenderContext::renderer.reset();
	RenderContext::swapchain.reset();
}

int main()
{
	glfwInit();
	
	glfwSetErrorCallback([](int code, const char* message) {
		std::cout << "GLFW Error" << code << ": " << message << std::endl;
	});
	
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

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
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, true);
	
	RenderContext::vkContext = std::make_unique<VKContext>(window);
	
	Camera camera({0, 0, 0});
	
	double previousTime = 0;
	while (!glfwWindowShouldClose(window))
	{
		double currentTime = glfwGetTime();
		double deltaTime = std::min(currentTime - previousTime, 0.1);
		previousTime = currentTime;
		
		glfwPollEvents();
		
		int width;
		int height;
		glfwGetFramebufferSize(window, &width, &height);
		while (width == 0 || height == 0)
		{
			glfwWaitEvents();
			glfwGetFramebufferSize(window, &width, &height);
		}
		
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, true);
		}
		
		camera.update(deltaTime);
		
		if (!RenderContext::swapchain)
		{
			RenderContext::swapchain = std::make_unique<VKSwapchain>();
			camera.setScreenSize({RenderContext::swapchain->getExtent().width, RenderContext::swapchain->getExtent().height});
		}
		if (!RenderContext::renderer)
		{
			RenderContext::renderer = std::make_unique<Renderer>();
		}
		
		RenderContext::renderer->draw(camera, deltaTime);
	}
	
	RenderContext::vkContext->getDevice().waitIdle();
	
	RenderContext::renderer.reset();
	RenderContext::swapchain.reset();
	RenderContext::vkContext.reset();
	
	glfwTerminate();
	
	return 0;
}