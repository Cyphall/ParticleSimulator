#include "VKDescriptorSetLayoutInfo.h"

VKDescriptorSetLayoutInfo& VKDescriptorSetLayoutInfo::registerBinding(uint32_t binding, vk::DescriptorType type, uint32_t count, vk::DescriptorBindingFlags flags)
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