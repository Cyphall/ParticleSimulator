#include "VKDescriptorSetLayout.h"

#include <ParticleSimulator/RenderContext.h>
#include <ParticleSimulator/VKObject/VKContext.h>

VKDescriptorSetLayout::VKDescriptorSetLayout(std::unordered_map<uint32_t, VKDescriptorSetLayoutBinding>&& bindingInfos):
	_bindingsInfo(bindingInfos)
{
	std::vector<vk::DescriptorSetLayoutBinding> vkBindings;
	vkBindings.reserve(_bindingsInfo.size());
	std::vector<vk::DescriptorBindingFlags> vkBindingsFlags;
	vkBindingsFlags.reserve(_bindingsInfo.size());
	
	for (const auto& [binding, bindingInfo] : _bindingsInfo)
	{
		vk::DescriptorSetLayoutBinding& vkBinding = vkBindings.emplace_back();
		vkBinding.binding = binding;
		vkBinding.descriptorType = bindingInfo.type;
		vkBinding.descriptorCount = bindingInfo.count;
		vkBinding.stageFlags = vk::ShaderStageFlagBits::eAll;
		vkBinding.pImmutableSamplers = nullptr; // Optional
		
		vkBindingsFlags.emplace_back(bindingInfo.flags);
	}
	
	vk::DescriptorSetLayoutBindingFlagsCreateInfo bindingFlagsCreateInfo;
	bindingFlagsCreateInfo.bindingCount = vkBindingsFlags.size();
	bindingFlagsCreateInfo.pBindingFlags = vkBindingsFlags.data();
	
	vk::DescriptorSetLayoutCreateInfo createInfo;
	createInfo.bindingCount = vkBindings.size();
	createInfo.pBindings = vkBindings.data();
	createInfo.pNext = &bindingFlagsCreateInfo;
	
	_handle = RenderContext::vkContext->getDevice().createDescriptorSetLayout(createInfo);
}

VKDescriptorSetLayout::~VKDescriptorSetLayout()
{
	RenderContext::vkContext->getDevice().destroyDescriptorSetLayout(_handle);
}

vk::DescriptorSetLayout VKDescriptorSetLayout::getHandle()
{
	return _handle;
}

const VKDescriptorSetLayoutBinding& VKDescriptorSetLayout::getBindingInfo(uint32_t binding) const
{
	return _bindingsInfo.at(binding);
}

const std::unordered_map<uint32_t, VKDescriptorSetLayoutBinding>& VKDescriptorSetLayout::getBindingInfos() const
{
	return _bindingsInfo;
}