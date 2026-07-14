#include "GravityViewPass.h"

#include <ParticleSimulator/Camera.h>
#include <ParticleSimulator/Rendering/ParticleData.h>

#include <CyphGPU/ComputePassContext.hpp>
#include <CyphGPU/ComputeShaderState.hpp>
#include <CyphGPU/ShaderTypes.hpp>

using namespace cgpu::shader_types;

GravityViewPass::GravityViewPass(const cgpu::DeviceSessionPtr& deviceSession):
	_deviceSession{deviceSession}
{
	createShaderState();
}

GravityViewPass::~GravityViewPass() {}

GravityViewPass::RenderOutput GravityViewPass::render(cgpu::CommandRecorder& rec, const RenderInput& input)
{
	rec.computePass({
		.callback = [&](cgpu::ComputePassContext& ctx) {
			ctx.bindPipelineStates(_computeShaderState);

			struct
			{
				float4x4 u_invViewProjectionMatrix;
				uint u_particleCount;
				ParticleData* u_inputParticles;
				WTexture2D<>::Handle u_outputImage;
				uint2 u_size;
			} parameters{};

			parameters.u_invViewProjectionMatrix = glm::inverse(input.camera->getProjection() * input.camera->getView());
			parameters.u_particleCount = input.particleCount;
			parameters.u_inputParticles = ctx.getBufferDevicePtr<ParticleData>(input.particlesBuffer, cgpu::CommandRecorder::ResourceAccess::eReadonly);
			parameters.u_outputImage = ctx.getStorageImageDescriptor(input.outputImage, cgpu::CommandRecorder::ResourceAccess::eReadWrite);
			parameters.u_size = glm::uvec2{input.outputImage->getDesc().extent};

			ctx.pushParameters(0, parameters);

			ctx.dispatch({cgpu::alignUp(parameters.u_size.get(), glm::uvec2{1024}) / 1024u, 1});
		},
	});

	return {};
}

void GravityViewPass::createShaderState()
{
	_computeShaderState = cgpu::ComputeShaderState::create(
		_deviceSession,
		{
			.compute_shader = {
				.source = "gravityView.slang",
			},
		}
	);
}
