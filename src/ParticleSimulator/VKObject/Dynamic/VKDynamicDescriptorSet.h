#pragma once

#include <memory>
#include <vector>
#include <vulkan/vulkan.hpp>

class VKDescriptorSetLayout;
class VKDescriptorSet;

class VKDynamicDescriptorSet
{
public:
	VKDynamicDescriptorSet(VKDescriptorSetLayout& layout);
	~VKDynamicDescriptorSet();
	
	operator VKDescriptorSet&();
	
	VKDescriptorSetLayout& getLayout();
	vk::DescriptorSet getHandle();
	
	void bindBuffer(uint32_t binding, vk::Buffer buffer, vk::DeviceSize offset, vk::DeviceSize range);
	void bindSampler(uint32_t binding, vk::Sampler sampler);

private:
	std::vector<std::unique_ptr<VKDescriptorSet>> _descriptorSets;
	
	VKDescriptorSet& get();
};