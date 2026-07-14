#include "ParticleViewPass.h"

#include <ParticleSimulator/Camera.h>
#include <ParticleSimulator/Rendering/ParticleData.h>

#include <CyphGPU/FragmentOutputState.hpp>
#include <CyphGPU/FragmentShaderState.hpp>
#include <CyphGPU/GraphicsPassContext.hpp>
#include <CyphGPU/PreRasterizationShaderState.hpp>
#include <CyphGPU/ShaderTypes.hpp>
#include <CyphGPU/VertexInputState.hpp>
#include <glm/glm.hpp>

using namespace cgpu::shader_types;

ParticleViewPass::ParticleViewPass(const cgpu::DeviceSessionPtr& deviceSession, vk::Format format):
	_deviceSession{deviceSession}
{
	createShaderStates(format);
}

ParticleViewPass::~ParticleViewPass() {}

ParticleViewPass::RenderOutput ParticleViewPass::render(cgpu::CommandRecorder& rec, const RenderInput& input)
{
	rec.graphicsPass({
		.color_attachments = {{{
			.image = input.outputImage,
			.load_op = vk::AttachmentLoadOp::eClear,
			.store_op = vk::AttachmentStoreOp::eStore,
			.clear_color_value = glm::vec4{0.0f, 0.0f, 0.0f, 1.0f},
		}}},
		.callback = [&](cgpu::GraphicsPassContext& ctx) {
			ctx.bindPipelineStates(
				_vertexInputState,
				_preRasterizationShaderState,
				_fragmentShaderState,
				_fragmentOutputState
			);

			struct
			{
				float4x4 u_viewProjectionMatrix;
				uint u_particleCount;
				ParticleData* u_inputParticles;
			} parameters{};

			parameters.u_viewProjectionMatrix = input.camera->getProjection() * input.camera->getView();
			parameters.u_particleCount = input.particleCount;
			parameters.u_inputParticles = ctx.getBufferDevicePtr<ParticleData>(input.particlesBuffer, cgpu::CommandRecorder::ResourceAccess::eReadonly);

			ctx.pushParameters(0, parameters);

			ctx.draw(input.particleCount, 1, 0, 0);
		},
	});

	return {};
}

void ParticleViewPass::createShaderStates(vk::Format format)
{
	_vertexInputState = cgpu::VertexInputState::create(
		_deviceSession,
		{
			.topology = vk::PrimitiveTopology::ePointList,
		}
	);

	_preRasterizationShaderState = cgpu::PreRasterizationShaderState::create(
		_deviceSession,
		{
			.vertex_shader = {
				.source = "particleView.slang",
			},
		}
	);

	_fragmentShaderState = cgpu::FragmentShaderState::create(
		_deviceSession,
		{
			.fragment_shader = {{
				.source = "particleView.slang",
			}},
		}
	);

	_fragmentOutputState = cgpu::FragmentOutputState::create(
		_deviceSession,
		{
			.color_attachments = {
				{
					.format = format,
				}
			},
		}
	);
}
