#include "Renderer.h"

#include <ParticleSimulator/RenderContext.h>
#include <ParticleSimulator/VKObject/CommandBuffer/VKCommandBuffer.h>
#include <ParticleSimulator/VKObject/Semaphore/VKSemaphore.h>
#include <ParticleSimulator/VKObject/Image/VKSwapchainImage.h>
#include <ParticleSimulator/VKObject/Buffer/VKBuffer.h>
#include <ParticleSimulator/VKObject/Queue/VKQueue.h>
#include <ParticleSimulator/Rendering/RenderPass/ComputePass.h>
#include <ParticleSimulator/Rendering/RenderPass/ParticleViewPass.h>
#include <ParticleSimulator/Rendering/RenderPass/GravityViewPass.h>
#include <ParticleSimulator/Camera.h>

#include <GLFW/glfw3.h>
#include <random>
#include <glm/gtx/rotate_vector.hpp>

static const uint32_t MAX_PARTICLES = 1000000;

static std::random_device device;
static std::mt19937 rng(device());
static std::uniform_real_distribution<float> posDirectionDist(0, 360);
static std::uniform_real_distribution<float> posLengthDist(0, 1);
static std::uniform_real_distribution<float> velocityDist(-0.1f, 0.1f);

Renderer::Renderer()
{
	createCommandBuffer();
	createSemaphore();
	createBuffers();
	
	_computePass = std::make_unique<ComputePass>();
	_particleViewPass = std::make_unique<ParticleViewPass>();
	_gravityViewPass = std::make_unique<GravityViewPass>();
}

Renderer::~Renderer()
{
	
}

void Renderer::draw(Camera& camera, float deltaTime)
{
	auto [currentSwapchainImage, imageAvailableSemaphore] = RenderContext::swapchain->retrieveNextImage();
	
	const VKPtr<VKCommandBuffer>& currentCommandBuffer = *_commandBuffer;
	
	currentCommandBuffer->begin();
	
	const VKPtr<VKBuffer<ParticleData>>& inputBuffer = _inputBufferIs1 ? _particlesBuffer1 : _particlesBuffer2;
	const VKPtr<VKBuffer<ParticleData>>& outputBuffer = _inputBufferIs1 ? _particlesBuffer2 : _particlesBuffer1;
	
	glm::vec2 mouseWorldPos = transformMousePos(camera);
	glm::vec2 mouseWorldDelta = {0.0f, 0.0f};
	if (_hasPreviousMouseWorldPos)
	{
//		mouseWorldDelta = mouseWorldPos - _previousMouseWorldPos;
	}
	
	bool cursorGravityEnabled = false;
	bool leftClickPressed = glfwGetMouseButton(RenderContext::vkContext->getWindow(), GLFW_MOUSE_BUTTON_LEFT);
	bool rightClickPressed = glfwGetMouseButton(RenderContext::vkContext->getWindow(), GLFW_MOUSE_BUTTON_RIGHT);
	if (leftClickPressed || rightClickPressed)
	{
		if (leftClickPressed)
		{
			uint32_t addedParticleCount = (MAX_PARTICLES / 5) * deltaTime;
			
			ParticleData* ptr = inputBuffer->map();
			
			for (uint32_t i = 0; i < addedParticleCount; i++)
			{
				uint32_t index = (_particleEnd + i) % MAX_PARTICLES;
				
				glm::vec2 particleRelativePos = glm::rotate(glm::vec2(0, 1), glm::radians(posDirectionDist(rng))) * posLengthDist(rng);
				
				ParticleData data{
					.position = mouseWorldPos + particleRelativePos,
					.velocity = mouseWorldDelta + glm::vec2(velocityDist(rng), velocityDist(rng))
				};
				
				std::memcpy(ptr + index, &data, sizeof(ParticleData));
			}
			
			inputBuffer->unmap();
			
			_particleCount = std::min(_particleCount + addedParticleCount, MAX_PARTICLES);
			_particleEnd = (_particleEnd + addedParticleCount) % MAX_PARTICLES;
		}
		
		if (rightClickPressed)
		{
			cursorGravityEnabled = true;
		}
	}
	
	bool particlesGravityKeyPressed = glfwGetKey(RenderContext::vkContext->getWindow(), GLFW_KEY_G) == GLFW_PRESS;
	if (particlesGravityKeyPressed && !_particlesGravityKeyPressedLastFrame)
	{
		_particlesGravityEnabled = !_particlesGravityEnabled;
	}
	
	ComputePass::RenderInput computeInput{};
	computeInput.deltaTime = deltaTime;
	computeInput.particlesInputBuffer = &inputBuffer;
	computeInput.particlesOutputBuffer = &outputBuffer;
	computeInput.particleCount = _particleCount;
	computeInput.cursorGravityEnabled = cursorGravityEnabled;
	computeInput.cursorGravityPos = mouseWorldPos;
	computeInput.particlesGravityEnabled = _particlesGravityEnabled;
	
	_computePass->render(currentCommandBuffer, computeInput);

#if 1 // 1: classic point-list particle view, 0: gravity view
	currentCommandBuffer->bufferMemoryBarrier(
		outputBuffer,
		vk::PipelineStageFlagBits2::eComputeShader,
		vk::AccessFlagBits2::eShaderStorageWrite,
		vk::PipelineStageFlagBits2::eVertexAttributeInput,
		vk::AccessFlagBits2::eVertexAttributeRead);
	
	currentCommandBuffer->imageMemoryBarrier(
		currentSwapchainImage,
		vk::PipelineStageFlagBits2::eNone,
		vk::AccessFlagBits2::eNone,
		vk::PipelineStageFlagBits2::eColorAttachmentOutput,
		vk::AccessFlagBits2::eColorAttachmentWrite,
		vk::ImageLayout::eColorAttachmentOptimal);
	
	ParticleViewPass::RenderInput particleViewInput;
	particleViewInput.camera = &camera;
	particleViewInput.particlesBuffer = &outputBuffer;
	particleViewInput.particleCount = _particleCount;
	particleViewInput.outputImage = currentSwapchainImage;
	
	_particleViewPass->render(currentCommandBuffer, particleViewInput);
	
	currentCommandBuffer->imageMemoryBarrier(
		currentSwapchainImage,
		vk::PipelineStageFlagBits2::eColorAttachmentOutput,
		vk::AccessFlagBits2::eColorAttachmentWrite,
		vk::PipelineStageFlagBits2::eNone,
		vk::AccessFlagBits2::eNone,
		vk::ImageLayout::ePresentSrcKHR);
#else
	currentCommandBuffer->bufferMemoryBarrier(
		outputBuffer,
		vk::PipelineStageFlagBits2::eComputeShader,
		vk::AccessFlagBits2::eShaderStorageWrite,
		vk::PipelineStageFlagBits2::eComputeShader,
		vk::AccessFlagBits2::eShaderStorageRead);
	
	currentCommandBuffer->imageMemoryBarrier(
		currentSwapchainImage,
		vk::PipelineStageFlagBits2::eNone,
		vk::AccessFlagBits2::eNone,
		vk::PipelineStageFlagBits2::eComputeShader,
		vk::AccessFlagBits2::eShaderStorageWrite,
		vk::ImageLayout::eGeneral);
	
	GravityViewPass::RenderInput gravityViewInput;
	gravityViewInput.camera = &camera;
	gravityViewInput.particlesBuffer = &outputBuffer;
	gravityViewInput.particleCount = _particleCount;
	gravityViewInput.outputImage = currentSwapchainImage;
	
	_gravityViewPass->render(currentCommandBuffer, gravityViewInput);
	
	currentCommandBuffer->imageMemoryBarrier(
		currentSwapchainImage,
		vk::PipelineStageFlagBits2::eComputeShader,
		vk::AccessFlagBits2::eShaderStorageWrite,
		vk::PipelineStageFlagBits2::eNone,
		vk::AccessFlagBits2::eNone,
		vk::ImageLayout::ePresentSrcKHR);
#endif
	
	currentCommandBuffer->end();
	
	const VKPtr<VKSemaphore>& renderFinishedSemaphore = *_renderFinishedSemaphore;
	
	RenderContext::vkContext->getQueue().submit(
		currentCommandBuffer,
		vk::PipelineStageFlagBits::eColorAttachmentOutput,
		&imageAvailableSemaphore,
		&renderFinishedSemaphore);
	
	RenderContext::vkContext->getQueue().present(*RenderContext::swapchain, &renderFinishedSemaphore);
	
	
	RenderContext::vkContext->getDevice().waitIdle();
	
	_inputBufferIs1 = !_inputBufferIs1;
	
	_previousMouseWorldPos = mouseWorldPos;
	_hasPreviousMouseWorldPos = true;
	
	_particlesGravityKeyPressedLastFrame = particlesGravityKeyPressed;
}

void Renderer::createCommandBuffer()
{
	_commandBuffer = VKCommandBuffer::createDynamic(*RenderContext::vkContext, *RenderContext::swapchain);
}

void Renderer::createSemaphore()
{
	vk::SemaphoreCreateInfo semaphoreCreateInfo;
	_renderFinishedSemaphore = VKSemaphore::createDynamic(*RenderContext::vkContext, *RenderContext::swapchain, semaphoreCreateInfo);
}

void Renderer::createBuffers()
{
	_particlesBuffer1 = VKBuffer<ParticleData>::create(
		*RenderContext::vkContext,
		vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eVertexBuffer,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eDeviceLocal
	);
	_particlesBuffer1->resize(MAX_PARTICLES);
	
	_particlesBuffer2 = VKBuffer<ParticleData>::create(
		*RenderContext::vkContext,
		vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eVertexBuffer,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eDeviceLocal
	);
	_particlesBuffer2->resize(MAX_PARTICLES);
}

static glm::vec2 remap(glm::vec2 value, glm::vec2 low1, glm::vec2 high1, glm::vec2 low2, glm::vec2 high2)
{
	return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
}

glm::vec2 Renderer::transformMousePos(Camera& camera)
{
	glm::dvec2 mousePosScreenCoord;
	glfwGetCursorPos(RenderContext::vkContext->getWindow(), &mousePosScreenCoord.x, &mousePosScreenCoord.y);
	
	glm::ivec2 windowSize;
	glfwGetWindowSize(RenderContext::vkContext->getWindow(), &windowSize.x, &windowSize.y);
	
	glm::vec4 mousePos4D(remap(mousePosScreenCoord, {0, 0}, windowSize, {-1, -1}, {1, 1}), 0, 1);
	
	glm::mat4 vpInverse = glm::inverse(camera.getProjection() * camera.getView());
	
	mousePos4D = vpInverse * mousePos4D;
	mousePos4D.x /= mousePos4D.w;
	mousePos4D.y /= mousePos4D.w;
	mousePos4D.z /= mousePos4D.w;
	
	return {mousePos4D.x, mousePos4D.y};
}