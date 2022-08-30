#pragma once

#include <ParticleSimulator/Rendering/ParticleStructs.h>

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

template<typename T>
class VKBuffer;
class VKDescriptorSetLayout;
class VKDescriptorSet;
class VKComputePipeline;

class ComputePass
{
public:
	struct RenderInput
	{
		float deltaTime;
		VKBuffer<ParticleData>* particlesInputBuffer = nullptr;
		VKBuffer<ParticleData>* particlesOutputBuffer = nullptr;
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
	
	RenderOutput render(vk::CommandBuffer commandBuffer, const RenderInput& input);

private:
	struct PushConstantData
	{
		float deltaTime;
		uint32_t particleCount;
		glm::vec2 cursorGravityPos;
		uint32_t cursorGravityEnabled; // bool
		uint32_t particlesGravityEnabled; // bool
	};
	
	std::unique_ptr<VKDescriptorSetLayout> _particlesDescriptorSetLayout;
	std::unique_ptr<VKDescriptorSet> _particlesDescriptorSet;
	
	std::unique_ptr<VKComputePipeline> _pipeline;
	
	void createDescriptorSetsLayout();
	void createDescriptorSets();
	void createPipeline();
};