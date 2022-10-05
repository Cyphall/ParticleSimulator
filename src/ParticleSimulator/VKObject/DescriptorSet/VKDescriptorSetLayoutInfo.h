#pragma once

#include <ParticleSimulator/VKObject/DescriptorSet/VKDescriptorSetLayoutBinding.h>
#include <ParticleSimulator/VKObject/DescriptorSet/VKDescriptorSetLayout.h>

#include <vulkan/vulkan.hpp>
#include <memory>

class VKDescriptorSetLayoutInfo
{
public:
	VKDescriptorSetLayoutInfo& registerBinding(uint32_t binding, vk::DescriptorType type, uint32_t count, vk::DescriptorBindingFlags flags = {});
	
private:
	friend class VKDescriptorSetLayout;
	
	std::unordered_map<uint32_t, VKDescriptorSetLayoutBinding> _bindings;
};