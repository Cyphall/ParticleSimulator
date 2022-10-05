#pragma once

#include <ParticleSimulator/VKObject/VKObject.h>

#include <vulkan/vulkan.hpp>

template<typename T>
class VKDynamic;
class VKSwapchain;
class VKPipelineLayoutInfo;
class VKDescriptorSetLayout;

class VKPipelineLayout : public VKObject
{
public:
	static VKPtr<VKPipelineLayout> create(VKContext& context, const VKPipelineLayoutInfo& pipelineLayoutInfo);
	static std::unique_ptr<VKDynamic<VKPipelineLayout>> createDynamic(VKContext& context, VKSwapchain& swapchain, const VKPipelineLayoutInfo& pipelineLayoutInfo);
	
	~VKPipelineLayout() override;
	
	vk::PipelineLayout getHandle();

protected:
	VKPipelineLayout(VKContext& context, const VKPipelineLayoutInfo& pipelineLayoutInfo);
	
	vk::PipelineLayout _pipelineLayout;
	std::vector<VKPtr<VKDescriptorSetLayout>> _descriptorSetsLayouts;
};