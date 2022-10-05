#pragma once

#include <ParticleSimulator/VKObject/VKPtr.h>
#include <ParticleSimulator/Rendering/ParticleStructs.h>

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

class Camera;
template<typename>
class VKBuffer;
class VKImageBase;
class VKDescriptorSetLayout;
class VKDescriptorSet;
class VKPipelineLayout;
class VKComputePipeline;
class VKCommandBuffer;

class GravityViewPass
{
public:
	struct RenderInput
	{
		Camera* camera = nullptr;
		const VKPtr<VKBuffer<ParticleData>>* particlesBuffer = nullptr;
		uint32_t particleCount;
		VKPtr<VKImageBase> outputImage;
	};
	
	struct RenderOutput
	{
		
	};
	
	GravityViewPass();
	~GravityViewPass();
	
	RenderOutput render(const VKPtr<VKCommandBuffer>& commandBuffer, const RenderInput& input);

private:
	struct PushConstantData
	{
		glm::mat4 vp;
		uint32_t particleCount;
	};
	
	VKPtr<VKDescriptorSetLayout> _particlesDescriptorSetLayout;
	VKPtr<VKDescriptorSet> _particlesDescriptorSet;
	
	VKPtr<VKPipelineLayout> _pipelineLayout;
	VKPtr<VKComputePipeline> _pipeline;
	
	void createDescriptorSetsLayout();
	void createDescriptorSets();
	void createPipelineLayout();
	void createPipeline();
};