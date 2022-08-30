#pragma once

#include <ParticleSimulator/Rendering/ParticleStructs.h>

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

class Camera;
template<typename T>
class VKBuffer;
class VKImageBase;
class VKDescriptorSetLayout;
class VKDescriptorSet;
class VKComputePipeline;

class GravityViewPass
{
public:
	struct RenderInput
	{
		Camera* camera = nullptr;
		VKBuffer<ParticleData>* particlesBuffer = nullptr;
		uint32_t particleCount;
		VKImageBase* outputImage = nullptr;
	};
	
	struct RenderOutput
	{
		
	};
	
	GravityViewPass();
	~GravityViewPass();
	
	RenderOutput render(vk::CommandBuffer commandBuffer, const RenderInput& input);

private:
	struct PushConstantData
	{
		glm::mat4 vp;
		uint32_t particleCount;
	};
	
	std::unique_ptr<VKDescriptorSetLayout> _particlesDescriptorSetLayout;
	std::unique_ptr<VKDescriptorSet> _particlesDescriptorSet;
	
	std::unique_ptr<VKComputePipeline> _pipeline;
	
	void createDescriptorSetsLayout();
	void createDescriptorSets();
	void createPipeline();
};