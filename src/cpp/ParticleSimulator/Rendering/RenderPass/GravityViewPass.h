#pragma once

#include <CyphGPU/CommandRecorder.hpp>
#include <CyphGPU/fwd.hpp>
#include <vulkan/vulkan.hpp>

class Camera;

class GravityViewPass
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

	explicit GravityViewPass(const cgpu::DeviceSessionPtr& deviceSession);
	~GravityViewPass();

	RenderOutput render(cgpu::CommandRecorder& rec, const RenderInput& input);

private:
	cgpu::DeviceSessionPtr _deviceSession;

	cgpu::ComputeShaderStatePtr _computeShaderState;

	void createShaderState();
};
