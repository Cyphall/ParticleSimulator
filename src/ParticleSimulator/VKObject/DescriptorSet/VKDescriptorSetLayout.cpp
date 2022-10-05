#include "VKDescriptorSetLayout.h"

#include <ParticleSimulator/VKObject/VKContext.h>
#include <ParticleSimulator/VKObject/VKDynamic.h>
#include <ParticleSimulator/VKObject/DescriptorSet/VKDescriptorSetLayoutInfo.h>

VKPtr<VKDescriptorSetLayout> VKDescriptorSetLayout::create(VKContext& context, const VKDescriptorSetLayoutInfo& descriptorSetLayoutInfo)
{
	return VKPtr<VKDescriptorSetLayout>(new VKDescriptorSetLayout(context, descriptorSetLayoutInfo));
}

std::unique_ptr<VKDynamic<VKDescriptorSetLayout>> VKDescriptorSetLayout::createDynamic(VKContext& context, VKSwapchain& swapchain, const VKDescriptorSetLayoutInfo& descriptorSetLayoutInfo)
{
	return std::unique_ptr<VKDynamic<VKDescriptorSetLayout>>(new VKDynamic<VKDescriptorSetLayout>(context, swapchain, descriptorSetLayoutInfo));
}

VKDescriptorSetLayout::VKDescriptorSetLayout(VKContext& context, const VKDescriptorSetLayoutInfo& descriptorSetLayoutInfo):
	VKObject(context), _bindingsInfo(descriptorSetLayoutInfo._bindings)
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
	
	_handle = _context.getDevice().createDescriptorSetLayout(createInfo);
}

VKDescriptorSetLayout::~VKDescriptorSetLayout()
{
	_context.getDevice().destroyDescriptorSetLayout(_handle);
}

vk::DescriptorSetLayout& VKDescriptorSetLayout::getHandle()
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