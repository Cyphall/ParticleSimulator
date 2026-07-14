#pragma once

#include <CyphGPU/CommandRecorder.hpp>
#include <CyphGPU/fwd.hpp>
#include <vulkan/vulkan.hpp>

class Camera;

class ParticleViewPass
{
public:
	struct RenderInput
	{
		Camera* camera = nullptr;
		cgpu::BufferPtr particlesBuffer;
		uint32_t particleCount;
		cgpu::ImagePtr outputImage;
	};

	struct RenderOutput
	{};

	explicit ParticleViewPass(const cgpu::DeviceSessionPtr& deviceSession, vk::Format format);
	~ParticleViewPass();

	RenderOutput render(cgpu::CommandRecorder& rec, const RenderInput& input);

private:
	cgpu::DeviceSessionPtr _deviceSession;

	cgpu::VertexInputStatePtr _vertexInputState;
	cgpu::PreRasterizationShaderStatePtr _preRasterizationShaderState;
	cgpu::FragmentShaderStatePtr _fragmentShaderState;
	cgpu::FragmentOutputStatePtr _fragmentOutputState;

	void createShaderStates(vk::Format format);
};
