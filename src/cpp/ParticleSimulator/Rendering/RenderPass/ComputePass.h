#pragma once

#include <CyphGPU/CommandRecorder.hpp>
#include <CyphGPU/fwd.hpp>
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

class ComputePass
{
public:
	struct RenderInput
	{
		float deltaTime;
		cgpu::BufferPtr particlesInputBuffer;
		cgpu::BufferPtr particlesOutputBuffer;
		uint32_t particleCount;
		bool cursorGravityEnabled;
		glm::vec2 cursorGravityPos;
		bool particlesGravityEnabled;
	};

	struct RenderOutput
	{};

	explicit ComputePass(const cgpu::DeviceSessionPtr& deviceSession);
	~ComputePass();

	RenderOutput render(cgpu::CommandRecorder& rec, const RenderInput& input);

private:
	cgpu::DeviceSessionPtr _deviceSession;

	cgpu::ComputeShaderStatePtr _computeShaderState;

	void createShaderState();
};
