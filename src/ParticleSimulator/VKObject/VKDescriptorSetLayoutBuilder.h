#pragma once

#include <ParticleSimulator/VKObject/VKDescriptorSetLayoutBinding.h>
#include <ParticleSimulator/VKObject/VKDescriptorSetLayout.h>

#include <vulkan/vulkan.hpp>
#include <memory>

class VKDescriptorSetLayoutBuilder
{
public:
	VKDescriptorSetLayoutBuilder& registerBinding(uint32_t binding, vk::DescriptorType type, uint32_t count, vk::DescriptorBindingFlags flags = {});
	
	std::unique_ptr<VKDescriptorSetLayout> build();
	
private:
	std::unordered_map<uint32_t, VKDescriptorSetLayoutBinding> _bindings;
};