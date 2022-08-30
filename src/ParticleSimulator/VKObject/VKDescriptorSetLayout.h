#pragma once

#include <ParticleSimulator/VKObject/VKDescriptorSetLayoutBinding.h>

#include <vulkan/vulkan.hpp>
#include <unordered_map>

class VKDescriptorSetLayout
{
public:
	~VKDescriptorSetLayout();
	
	VKDescriptorSetLayout(const VKDescriptorSetLayout& other) = delete;
	VKDescriptorSetLayout& operator=(const VKDescriptorSetLayout& other) = delete;
	
	VKDescriptorSetLayout(VKDescriptorSetLayout&& other) = delete;
	VKDescriptorSetLayout& operator=(VKDescriptorSetLayout&& other) = delete;
	
	vk::DescriptorSetLayout getHandle();
	const VKDescriptorSetLayoutBinding& getBindingInfo(uint32_t binding) const;
	const std::unordered_map<uint32_t, VKDescriptorSetLayoutBinding>& getBindingInfos() const;

private:
	friend class VKDescriptorSetLayoutBuilder;
	
	VKDescriptorSetLayout(std::unordered_map<uint32_t, VKDescriptorSetLayoutBinding>&& bindingInfos);
	
	std::unordered_map<uint32_t, VKDescriptorSetLayoutBinding> _bindingsInfo;
	vk::DescriptorSetLayout _handle;
};