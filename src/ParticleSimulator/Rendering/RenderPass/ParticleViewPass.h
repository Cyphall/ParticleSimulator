#pragma once

#include <ParticleSimulator/VKObject/VKPtr.h>
#include <ParticleSimulator/Rendering/ParticleStructs.h>

#include <vulkan/vulkan.hpp>

class Camera;
template<typename>
class VKBuffer;
class VKImageBase;
class VKDescriptorSetLayout;
class VKDescriptorSet;
class VKPipelineLayout;
class VKGraphicsPipeline;
class VKCommandBuffer;

class ParticleViewPass
{
public:
	struct RenderInput
	{
		Camera* camera = nullptr;
		const VKPtr<VKBuffer<ParticleData>>* particlesBuffer = nullptr;
		uint32_t particleCount = 0;
		VKPtr<VKImageBase> outputImage;
	};
	
	struct RenderOutput
	{
		
	};
	
	ParticleViewPass();
	~ParticleViewPass();
	
	RenderOutput render(const VKPtr<VKCommandBuffer>& commandBuffer, const RenderInput& input);
	
private:
	struct PushConstantData
	{
		glm::mat4 vp;
	};
	
	VKPtr<VKPipelineLayout> _pipelineLayout;
	VKPtr<VKGraphicsPipeline> _pipeline;
	
	void createPipelineLayout();
	void createPipeline();
};