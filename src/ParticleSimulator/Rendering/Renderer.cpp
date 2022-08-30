#include "Renderer.h"

#include <ParticleSimulator/VKObject/VKSwapchain.h>
#include <ParticleSimulator/RenderContext.h>
#include <ParticleSimulator/VKObject/Dynamic/VKDynamicCommandBuffer.h>
#include <ParticleSimulator/VKObject/Dynamic/VKDynamicSemaphore.h>
#include <ParticleSimulator/VKObject/Dynamic/VKDynamicFence.h>
#include <ParticleSimulator/Rendering/RenderPass/ComputePass.h>
#include <ParticleSimulator/Rendering/RenderPass/ParticleViewPass.h>
#include <ParticleSimulator/Rendering/RenderPass/GravityViewPass.h>
#include <ParticleSimulator/VKObject/VKSwapchainImage.h>
#include <ParticleSimulator/VKObject/VKBuffer.h>
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
	createCommandPool();
	createCommandBuffer();
	createSemaphore();
	createFence();
	createBuffers();
	
	_computePass = std::make_unique<ComputePass>();
	_particleViewPass = std::make_unique<ParticleViewPass>();
	_gravityViewPass = std::make_unique<GravityViewPass>();
}

Renderer::~Renderer()
{
	_gravityViewPass.reset();
	_particleViewPass.reset();
	_computePass.reset();
	
	_renderFinishedFence.reset();
	_renderFinishedSemaphore.reset();
	_commandBuffer.reset();
	RenderContext::vkContext->getDevice().destroyCommandPool(_commandPool);
}

void Renderer::draw(Camera& camera, float deltaTime)
{
	auto [currentSwapchainImage, imageAvailableSemaphore] = RenderContext::swapchain->retrieveNextImage();
	
	vk::Fence dynamicResourcesAvailableFence = _renderFinishedFence->get();
	
	if (RenderContext::vkContext->getDevice().waitForFences(dynamicResourcesAvailableFence, true, UINT64_MAX) != vk::Result::eSuccess)
	{
		throw;
	}
	RenderContext::vkContext->getDevice().resetFences(dynamicResourcesAvailableFence);
	
	vk::CommandBuffer currentCommandBuffer = _commandBuffer->get();
	
	vk::CommandBufferBeginInfo commandBufferBeginInfo;
	commandBufferBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
	commandBufferBeginInfo.pInheritanceInfo = nullptr; // Optional
	
	currentCommandBuffer.begin(commandBufferBeginInfo);
	
	VKBuffer<ParticleData>* inputBuffer = _inputBufferIs1 ? _particlesBuffer1.get() : _particlesBuffer2.get();
	VKBuffer<ParticleData>* outputBuffer = _inputBufferIs1 ? _particlesBuffer2.get() : _particlesBuffer1.get();
	
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
	computeInput.particlesInputBuffer = inputBuffer;
	computeInput.particlesOutputBuffer = outputBuffer;
	computeInput.particleCount = _particleCount;
	computeInput.cursorGravityEnabled = cursorGravityEnabled;
	computeInput.cursorGravityPos = mouseWorldPos;
	computeInput.particlesGravityEnabled = _particlesGravityEnabled;
	
	_computePass->render(currentCommandBuffer, computeInput);

#if 1 // 1: classic point-list particle view, 0: gravity view
	vk::MemoryBarrier2 memoryBarrier;
	memoryBarrier.srcStageMask = vk::PipelineStageFlagBits2::eComputeShader;
	memoryBarrier.srcAccessMask = vk::AccessFlagBits2::eShaderStorageWrite;
	memoryBarrier.dstStageMask = vk::PipelineStageFlagBits2::eVertexAttributeInput;
	memoryBarrier.dstAccessMask = vk::AccessFlagBits2::eVertexAttributeRead;
	
	vk::DependencyInfo dependencyInfo;
	dependencyInfo.memoryBarrierCount = 1;
	dependencyInfo.pMemoryBarriers = &memoryBarrier;
	
	currentCommandBuffer.pipelineBarrier2(dependencyInfo);
	
	currentSwapchainImage.transitionLayout(currentCommandBuffer, vk::ImageLayout::eColorAttachmentOptimal);
	
	ParticleViewPass::RenderInput particleViewInput;
	particleViewInput.camera = &camera;
	particleViewInput.particlesBuffer = outputBuffer;
	particleViewInput.particleCount = _particleCount;
	particleViewInput.outputImage = &currentSwapchainImage;
	
	_particleViewPass->render(currentCommandBuffer, particleViewInput);
#else
	vk::MemoryBarrier2 memoryBarrier;
	memoryBarrier.srcStageMask = vk::PipelineStageFlagBits2::eComputeShader;
	memoryBarrier.srcAccessMask = vk::AccessFlagBits2::eShaderStorageWrite;
	memoryBarrier.dstStageMask = vk::PipelineStageFlagBits2::eComputeShader;
	memoryBarrier.dstAccessMask = vk::AccessFlagBits2::eShaderStorageRead;
	
	vk::DependencyInfo dependencyInfo;
	dependencyInfo.memoryBarrierCount = 1;
	dependencyInfo.pMemoryBarriers = &memoryBarrier;
	
	currentCommandBuffer.pipelineBarrier2(dependencyInfo);
	
	currentSwapchainImage.transitionLayout(currentCommandBuffer, vk::ImageLayout::eGeneral);
	
	GravityViewPass::RenderInput gravityViewInput;
	gravityViewInput.camera = &camera;
	gravityViewInput.particlesBuffer = outputBuffer;
	gravityViewInput.particleCount = _particleCount;
	gravityViewInput.outputImage = &currentSwapchainImage;
	
	_gravityViewPass->render(currentCommandBuffer, gravityViewInput);
#endif
	
	currentSwapchainImage.transitionLayout(currentCommandBuffer, vk::ImageLayout::ePresentSrcKHR);
	
	currentCommandBuffer.end();
	
	vk::PipelineStageFlags waitStages = vk::PipelineStageFlagBits::eColorAttachmentOutput;
	
	vk::Semaphore renderFinishedSemaphore = _renderFinishedSemaphore->get();
	
	vk::SubmitInfo submitInfo;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &imageAvailableSemaphore;
	submitInfo.pWaitDstStageMask = &waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &currentCommandBuffer;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &renderFinishedSemaphore;
	
	RenderContext::vkContext->getMainQueue().submit(submitInfo, dynamicResourcesAvailableFence);
	
	RenderContext::swapchain->present(renderFinishedSemaphore);
	
	RenderContext::vkContext->getDevice().waitIdle();
	
	_inputBufferIs1 = !_inputBufferIs1;
	
	_previousMouseWorldPos = mouseWorldPos;
	_hasPreviousMouseWorldPos = true;
	
	_particlesGravityKeyPressedLastFrame = particlesGravityKeyPressed;
}

void Renderer::createCommandPool()
{
	vk::CommandPoolCreateInfo poolCreateInfo;
	poolCreateInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
	poolCreateInfo.queueFamilyIndex = RenderContext::vkContext->getMainQueueFamily();
	
	_commandPool = RenderContext::vkContext->getDevice().createCommandPool(poolCreateInfo);
}

void Renderer::createCommandBuffer()
{
	_commandBuffer = std::make_unique<VKDynamicCommandBuffer>(_commandPool, vk::CommandBufferLevel::ePrimary);
}

void Renderer::createSemaphore()
{
	vk::SemaphoreCreateInfo semaphoreCreateInfo;
	_renderFinishedSemaphore = std::make_unique<VKDynamicSemaphore>(semaphoreCreateInfo);
}

void Renderer::createFence()
{
	vk::FenceCreateInfo fenceCreateInfo;
	fenceCreateInfo.flags |= vk::FenceCreateFlagBits::eSignaled;
	_renderFinishedFence = std::make_unique<VKDynamicFence>(fenceCreateInfo);
}

void Renderer::createBuffers()
{
	_particlesBuffer1 = std::make_unique<VKBuffer<ParticleData>>(
		vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eVertexBuffer,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eDeviceLocal
	);
	_particlesBuffer1->resize(MAX_PARTICLES);
	
	_particlesBuffer2 = std::make_unique<VKBuffer<ParticleData>>(
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