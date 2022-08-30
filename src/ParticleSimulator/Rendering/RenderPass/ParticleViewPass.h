#pragma once

#include <ParticleSimulator/Rendering/ParticleStructs.h>

#include <vulkan/vulkan.hpp>

class Camera;
template<typename T>
class VKBuffer;
class VKImageBase;
class VKDescriptorSetLayout;
class VKDescriptorSet;
class VKGraphicsPipeline;

class ParticleViewPass
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
	
	ParticleViewPass();
	~ParticleViewPass();
	
	RenderOutput render(vk::CommandBuffer commandBuffer, const RenderInput& input);
	
private:
	struct PushConstantData
	{
		glm::mat4 vp;
	};
	
	std::unique_ptr<VKGraphicsPipeline> _pipeline;
	
	void createPipeline();
};