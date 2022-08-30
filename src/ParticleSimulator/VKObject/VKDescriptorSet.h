#pragma once

#include <vulkan/vulkan.hpp>

class VKDescriptorSetLayout;

class VKDescriptorSet
{
public:
	VKDescriptorSet(VKDescriptorSetLayout& layout);
	~VKDescriptorSet();
	
	VKDescriptorSet(const VKDescriptorSet& other) = delete;
	VKDescriptorSet& operator=(const VKDescriptorSet& other) = delete;
	
	VKDescriptorSet(VKDescriptorSet&& other) = delete;
	VKDescriptorSet& operator=(VKDescriptorSet&& other) = delete;
	
	VKDescriptorSetLayout& getLayout();
	vk::DescriptorSet getHandle();
	
	void bindBuffer(uint32_t binding, vk::Buffer buffer, vk::DeviceSize offset = 0, vk::DeviceSize range = VK_WHOLE_SIZE, uint32_t arrayIndex = 0);
	void bindSampler(uint32_t binding, vk::Sampler sampler, uint32_t arrayIndex = 0);
	void bindImage(uint32_t binding, vk::ImageView imageView, vk::ImageLayout imageLayout, uint32_t arrayIndex = 0);
	
protected:
	VKDescriptorSetLayout& _layout;
	vk::DescriptorPool _descriptorPool;
	vk::DescriptorSet _descriptorSet;
};