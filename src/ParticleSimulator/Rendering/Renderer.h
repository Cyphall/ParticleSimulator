#pragma once

#include <ParticleSimulator/Rendering/ParticleStructs.h>
#include <ParticleSimulator/VKObject/VKPtr.h>

#include <vulkan/vulkan.hpp>
#include <memory>

class Camera;
template<typename T>
class VKDynamic;
class VKCommandBuffer;
class VKSemaphore;
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
	std::unique_ptr<VKDynamic<VKCommandBuffer>> _commandBuffer;
	
	std::unique_ptr<VKDynamic<VKSemaphore>> _renderFinishedSemaphore;
	
	std::unique_ptr<ComputePass> _computePass;
	std::unique_ptr<ParticleViewPass> _particleViewPass;
	std::unique_ptr<GravityViewPass> _gravityViewPass;
	
	VKPtr<VKBuffer<ParticleData>> _particlesBuffer1;
	VKPtr<VKBuffer<ParticleData>> _particlesBuffer2;
	
	uint32_t _particleCount = 0;
	uint32_t _particleEnd = 0;
	
	bool _inputBufferIs1 = true;
	
	bool _particlesGravityKeyPressedLastFrame = false;
	bool _particlesGravityEnabled = false;
	
	bool _hasPreviousMouseWorldPos = false;
	glm::vec2 _previousMouseWorldPos = {0, 0};
	
	static glm::vec2 transformMousePos(Camera& camera);
	
	void createCommandBuffer();
	void createSemaphore();
	void createBuffers();
};