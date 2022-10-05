#pragma once

#include <ParticleSimulator/VKObject/VKPtr.h>

#include <vector>
#include <optional>
#include <vulkan/vulkan.hpp>

class VKDescriptorSetLayout;

class VKPipelineLayoutInfo
{
public:
	void registerDescriptorSetLayout(const VKPtr<VKDescriptorSetLayout>& descriptorSetLayout);
	
	template<typename T>
	void registerPushConstantLayout(vk::ShaderStageFlags stageFlags)
	{
		vk::PushConstantRange pushConstantRange;
		pushConstantRange.stageFlags = stageFlags;
		pushConstantRange.size = sizeof(T);
		pushConstantRange.offset = 0;
		
		_pushConstantRange = pushConstantRange;
	}
	
private:
	friend class VKPipelineLayout;
	
	std::vector<vk::DescriptorSetLayout> _vkDescriptorSetsLayouts;
	std::vector<VKPtr<VKDescriptorSetLayout>> _descriptorSetsLayouts;
	std::optional<vk::PushConstantRange> _pushConstantRange;
};