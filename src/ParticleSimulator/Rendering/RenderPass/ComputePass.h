#pragma once

#include <ParticleSimulator/VKObject/VKPtr.h>
#include <ParticleSimulator/Rendering/ParticleStructs.h>

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

template<typename>
class VKBuffer;
class VKDescriptorSetLayout;
class VKDescriptorSet;
class VKPipelineLayout;
class VKComputePipeline;
class VKCommandBuffer;

class ComputePass
{
public:
	struct RenderInput
	{
		float deltaTime;
		const VKPtr<VKBuffer<ParticleData>>* particlesInputBuffer = nullptr;
		const VKPtr<VKBuffer<ParticleData>>* particlesOutputBuffer = nullptr;
		uint32_t particleCount;
		bool cursorGravityEnabled;
		glm::vec2 cursorGravityPos;
		bool particlesGravityEnabled;
	};
	
	struct RenderOutput
	{
	};
	
	ComputePass();
	~ComputePass();
	
	RenderOutput render(const VKPtr<VKCommandBuffer>& commandBuffer, const RenderInput& input);

private:
	struct PushConstantData
	{
		float deltaTime;
		uint32_t particleCount;
		glm::vec2 cursorGravityPos;
		uint32_t cursorGravityEnabled; // bool
		uint32_t particlesGravityEnabled; // bool
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