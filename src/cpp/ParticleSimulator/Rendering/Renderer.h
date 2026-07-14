#pragma once

#include <CyphGPU/CommandContext.hpp>
#include <CyphGPU/fwd.hpp>
#include <GLFW/glfw3.h>
#include <memory>
#include <vulkan/vulkan.hpp>

class Camera;
class ComputePass;
class ParticleViewPass;
class GravityViewPass;

class Renderer
{
public:
	explicit Renderer(const cgpu::DeviceSessionPtr& deviceSession, GLFWwindow* window, vk::Format format);
	~Renderer();

	void draw(Camera& camera, float deltaTime, const cgpu::ImagePtr& outputImage);

private:
	cgpu::DeviceSessionPtr _deviceSession;
	GLFWwindow* _window;
	cgpu::CommandContext _cmdCtx;

	std::unique_ptr<ComputePass> _computePass;
	std::unique_ptr<ParticleViewPass> _particleViewPass;
	std::unique_ptr<GravityViewPass> _gravityViewPass;

	cgpu::BufferPtr _particlesBufferSrc;
	cgpu::BufferPtr _particlesBufferDst;

	uint32_t _particleCount = 0;
	uint32_t _particleEnd = 0;

	bool _particlesGravityKeyPressedLastFrame = false;
	bool _particlesGravityEnabled = false;

	bool _hasPreviousMouseWorldPos = false;
	glm::vec2 _previousMouseWorldPos = {0, 0};

	glm::vec2 transformMousePos(Camera& camera);

	void createBuffers();
};
