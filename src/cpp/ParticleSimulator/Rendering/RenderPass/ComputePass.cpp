#include "ComputePass.h"

#include <ParticleSimulator/Rendering/ParticleData.h>

#include <CyphGPU/ComputePassContext.hpp>
#include <CyphGPU/ComputeShaderState.hpp>
#include <CyphGPU/ShaderTypes.hpp>

using namespace cgpu::shader_types;

ComputePass::ComputePass(const cgpu::DeviceSessionPtr& deviceSession):
	_deviceSession{deviceSession}
{
	createShaderState();
}

ComputePass::RenderOutput ComputePass::render(cgpu::CommandRecorder& rec, const RenderInput& input)
{
	rec.computePass({
		.callback = [&](cgpu::ComputePassContext& ctx) {
			struct
			{
				float u_deltaTime;
				uint u_particleCount;
				float2 u_cursorGravityPos;
				bool u_cursorGravityEnabled;
				bool u_particlesGravityEnabled;
				ParticleData* u_inputParticles;
				ParticleData* u_outputParticles;
			} parameters{};

			parameters.u_deltaTime = input.deltaTime;
			parameters.u_particleCount = input.particleCount;
			parameters.u_cursorGravityEnabled = input.cursorGravityEnabled;
			parameters.u_cursorGravityPos = input.cursorGravityPos;
			parameters.u_particlesGravityEnabled = input.particlesGravityEnabled;
			parameters.u_inputParticles = ctx.getBufferDevicePtr<ParticleData>(input.particlesInputBuffer, cgpu::StorageAccess::eReadonly);
			parameters.u_outputParticles = ctx.getBufferDevicePtr<ParticleData>(input.particlesOutputBuffer, cgpu::StorageAccess::eWriteonly);

			ctx.dispatch(_computeShaderState, {cgpu::alignUp(input.particleCount, 1024) / 1024, 1, 1}, parameters);
		},
	});

	return {};
}

void ComputePass::createShaderState()
{
	_computeShaderState = cgpu::ComputeShaderState::create(
		_deviceSession,
		{
			.compute_shader = {
				.source = "simulateParticles.slang",
			},
		}
	);
}
