#pragma once

#include <vector>
#include <optional>
#include <vulkan/vulkan.hpp>

class VKDescriptorSetLayout;

class VKPipelineLayoutInfo
{
public:
	void registerDescriptorSetLayout(VKDescriptorSetLayout& descriptorSetLayout);
	
	template<typename T>
	void registerPushConstantLayout(vk::ShaderStageFlags stageFlags)
	{
		vk::PushConstantRange pushConstantRange;
		pushConstantRange.stageFlags = stageFlags;
		pushConstantRange.size = sizeof(T);
		pushConstantRange.offset = 0;
		
		_pushConstantRange = pushConstantRange;
	}
	
	vk::PipelineLayoutCreateInfo get() const;
	
private:
	std::vector<vk::DescriptorSetLayout> _descriptorSetsLayout;
	std::optional<vk::PushConstantRange> _pushConstantRange;
};