#pragma once

#include <ParticleSimulator/Rendering/ParticleStructs.h>

#include <vulkan/vulkan.hpp>
#include <memory>

class Camera;
class VKDynamicCommandBuffer;
class VKDynamicSemaphore;
class VKDynamicFence;
class ComputePass;
class ParticleViewPass;
class GravityViewPass;
template<typename T>
class VKBuffer;

class Renderer
{
public:
	Renderer();
	~Renderer();
	
	void draw(Camera& camera, float deltaTime);
	
private:
	vk::CommandPool _commandPool;
	std::unique_ptr<VKDynamicCommandBuffer> _commandBuffer;
	
	std::unique_ptr<VKDynamicSemaphore> _renderFinishedSemaphore;
	std::unique_ptr<VKDynamicFence> _renderFinishedFence;
	
	std::unique_ptr<ComputePass> _computePass;
	std::unique_ptr<ParticleViewPass> _particleViewPass;
	std::unique_ptr<GravityViewPass> _gravityViewPass;
	
	std::unique_ptr<VKBuffer<ParticleData>> _particlesBuffer1;
	std::unique_ptr<VKBuffer<ParticleData>> _particlesBuffer2;
	
	uint32_t _particleCount = 0;
	uint32_t _particleEnd = 0;
	
	bool _inputBufferIs1 = true;
	
	bool _particlesGravityKeyPressedLastFrame = false;
	bool _particlesGravityEnabled = false;
	
	bool _hasPreviousMouseWorldPos = false;
	glm::vec2 _previousMouseWorldPos = {0, 0};
	
	static glm::vec2 transformMousePos(Camera& camera);
	
	void createCommandPool();
	void createCommandBuffer();
	void createSemaphore();
	void createFence();
	void createBuffers();
};