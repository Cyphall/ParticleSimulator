#include "Renderer.h"

#include <ParticleSimulator/Camera.h>
#include <ParticleSimulator/Rendering/ParticleData.h>
#include <ParticleSimulator/Rendering/RenderPass/ComputePass.h>
#include <ParticleSimulator/Rendering/RenderPass/GravityViewPass.h>
#include <ParticleSimulator/Rendering/RenderPass/ParticleViewPass.h>

#include <CyphGPU/Buffer.hpp>
#include <CyphGPU/DeviceSession.hpp>
#include <GLFW/glfw3.h>
#include <glm/gtx/rotate_vector.hpp>
#include <random>

static const uint32_t MAX_PARTICLES = 1000000;

static std::random_device device;
static std::mt19937 rng(device());
static std::uniform_real_distribution<float> posDirectionDist(0, 360);
static std::uniform_real_distribution<float> posLengthDist(0, 1);
static std::uniform_real_distribution<float> velocityDist(-0.1f, 0.1f);

Renderer::Renderer(const cgpu::DeviceSessionPtr& deviceSession, GLFWwindow* window, vk::Format format):
	_deviceSession{deviceSession},
	_window{window},
	_cmdCtx{deviceSession}
{
	createBuffers();

	_computePass = std::make_unique<ComputePass>(deviceSession);
	_particleViewPass = std::make_unique<ParticleViewPass>(deviceSession, format);
	_gravityViewPass = std::make_unique<GravityViewPass>(deviceSession);
}

Renderer::~Renderer() {}

void Renderer::draw(Camera& camera, float deltaTime, const cgpu::ImagePtr& outputImage)
{
	glm::vec2 mouseWorldPos = transformMousePos(camera);
	glm::vec2 mouseWorldDelta = {0.0f, 0.0f};
	if (_hasPreviousMouseWorldPos)
	{
		//		mouseWorldDelta = mouseWorldPos - _previousMouseWorldPos;
	}

	bool cursorGravityEnabled = false;
	bool leftClickPressed = glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT);
	bool rightClickPressed = glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_RIGHT);
	if (leftClickPressed || rightClickPressed)
	{
		if (leftClickPressed)
		{
			uint32_t addedParticleCount = (MAX_PARTICLES / 5) * deltaTime;

			ParticleData* ptr = _particlesBufferSrc->getHostPtr<ParticleData>();
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

			_particleCount = std::min(_particleCount + addedParticleCount, MAX_PARTICLES);
			_particleEnd = (_particleEnd + addedParticleCount) % MAX_PARTICLES;
		}

		if (rightClickPressed)
		{
			cursorGravityEnabled = true;
		}
	}

	bool particlesGravityKeyPressed = glfwGetKey(_window, GLFW_KEY_G) == GLFW_PRESS;
	if (particlesGravityKeyPressed && !_particlesGravityKeyPressedLastFrame)
	{
		_particlesGravityEnabled = !_particlesGravityEnabled;
	}

	auto rec = _cmdCtx.createRecorder(_deviceSession->getMainQueue());

	ComputePass::RenderInput computeInput{};
	computeInput.deltaTime = deltaTime;
	computeInput.particlesInputBuffer = _particlesBufferSrc;
	computeInput.particlesOutputBuffer = _particlesBufferDst;
	computeInput.particleCount = _particleCount;
	computeInput.cursorGravityEnabled = cursorGravityEnabled;
	computeInput.cursorGravityPos = mouseWorldPos;
	computeInput.particlesGravityEnabled = _particlesGravityEnabled;

	_computePass->render(rec, computeInput);

#if 1 // 1: classic point-list particle view, 0: gravity view
	rec.barrier({
		.src_stages = vk::PipelineStageFlagBits2::eComputeShader,
		.src_accesses = vk::AccessFlagBits2::eShaderStorageWrite,
		.dst_stages = vk::PipelineStageFlagBits2::eVertexShader,
		.dst_accesses = vk::AccessFlagBits2::eShaderStorageRead,
	});

	ParticleViewPass::RenderInput particleViewInput;
	particleViewInput.camera = &camera;
	particleViewInput.particlesBuffer = _particlesBufferDst;
	particleViewInput.particleCount = _particleCount;
	particleViewInput.outputImage = outputImage;

	_particleViewPass->render(rec, particleViewInput);
#else
	rec.barrier({
		.src_stages = vk::PipelineStageFlagBits2::eComputeShader,
		.src_accesses = vk::AccessFlagBits2::eShaderStorageWrite,
		vk::PipelineStageFlagBits2::eComputeShader,
		.dst_accesses = vk::AccessFlagBits2::eShaderStorageRead,
	});

	GravityViewPass::RenderInput gravityViewInput;
	gravityViewInput.camera = &camera;
	gravityViewInput.particlesBuffer = &outputBuffer;
	gravityViewInput.particleCount = _particleCount;
	gravityViewInput.outputImage = currentSwapchainImage;

	_gravityViewPass->render(rec, gravityViewInput);
#endif

	rec.submit();

	_deviceSession->waitIdle();

	std::swap(_particlesBufferSrc, _particlesBufferDst);

	_previousMouseWorldPos = mouseWorldPos;
	_hasPreviousMouseWorldPos = true;

	_particlesGravityKeyPressedLastFrame = particlesGravityKeyPressed;
}

void Renderer::createBuffers()
{
	_particlesBufferSrc = cgpu::Buffer::create(
		_deviceSession,
		{
			.name = "Particle buffer 1",
			.size = MAX_PARTICLES * sizeof(ParticleData),
			.memory_type = cgpu::MemoryType::eCPUVisibleGPU,
			.min_alignment = alignof(ParticleData),
		}
	);

	_particlesBufferDst = cgpu::Buffer::create(
		_deviceSession,
		{
			.name = "Particle buffer 2",
			.size = MAX_PARTICLES * sizeof(ParticleData),
			.memory_type = cgpu::MemoryType::eCPUVisibleGPU,
			.min_alignment = alignof(ParticleData),
		}
	);
}

static glm::vec2 remap(glm::vec2 value, glm::vec2 low1, glm::vec2 high1, glm::vec2 low2, glm::vec2 high2)
{
	return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
}

glm::vec2 Renderer::transformMousePos(Camera& camera)
{
	glm::dvec2 mousePosScreenCoord;
	glfwGetCursorPos(_window, &mousePosScreenCoord.x, &mousePosScreenCoord.y);

	glm::ivec2 windowSize;
	glfwGetWindowSize(_window, &windowSize.x, &windowSize.y);

	glm::vec4 mousePos4D(remap(mousePosScreenCoord, {0, 0}, windowSize, {-1, -1}, {1, 1}), 0, 1);

	glm::mat4 vpInverse = glm::inverse(camera.getProjection() * camera.getView());

	mousePos4D = vpInverse * mousePos4D;
	mousePos4D.x /= mousePos4D.w;
	mousePos4D.y /= mousePos4D.w;
	mousePos4D.z /= mousePos4D.w;

	return {mousePos4D.x, mousePos4D.y};
}
