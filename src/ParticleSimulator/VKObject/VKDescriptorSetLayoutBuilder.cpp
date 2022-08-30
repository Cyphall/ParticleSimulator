#include "VKDescriptorSetLayoutBuilder.h"

VKDescriptorSetLayoutBuilder& VKDescriptorSetLayoutBuilder::registerBinding(uint32_t binding, vk::DescriptorType type, uint32_t count, vk::DescriptorBindingFlags flags)
{
	auto it = _bindings.try_emplace(binding);
	if (!it.second)
	{
		throw;
	}
	
	VKDescriptorSetLayoutBinding& descriptorSetLayoutBinding = it.first->second;
	descriptorSetLayoutBinding.type = type;
	descriptorSetLayoutBinding.count = count;
	descriptorSetLayoutBinding.flags = flags;
	
	return *this;
}

std::unique_ptr<VKDescriptorSetLayout> VKDescriptorSetLayoutBuilder::build()
{
	return std::unique_ptr<VKDescriptorSetLayout>(new VKDescriptorSetLayout(std::move(_bindings)));
}