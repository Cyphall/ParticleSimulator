#include "VKDynamicDescriptorSet.h"

#include <ParticleSimulator/RenderContext.h>
#include <ParticleSimulator/VKObject/VKSwapchain.h>
#include <ParticleSimulator/VKObject/VKDescriptorSet.h>

VKDynamicDescriptorSet::VKDynamicDescriptorSet(VKDescriptorSetLayout& layout)
{
	size_t count = RenderContext::swapchain->getDynamicCount();
	for (size_t i = 0; i < count; i++)
	{
		_descriptorSets.push_back(std::make_unique<VKDescriptorSet>(layout));
	}
}

VKDynamicDescriptorSet::~VKDynamicDescriptorSet()
{
	
}

VKDynamicDescriptorSet::operator VKDescriptorSet&()
{
	return get();
}

VKDescriptorSetLayout& VKDynamicDescriptorSet::getLayout()
{
	return get().getLayout();
}

vk::DescriptorSet VKDynamicDescriptorSet::getHandle()
{
	return get().getHandle();
}

void VKDynamicDescriptorSet::bindBuffer(uint32_t binding, vk::Buffer buffer, vk::DeviceSize offset, vk::DeviceSize range)
{
	get().bindBuffer(binding, buffer, offset, range);
}

void VKDynamicDescriptorSet::bindSampler(uint32_t binding, vk::Sampler sampler)
{
	get().bindSampler(binding, sampler);
}

VKDescriptorSet& VKDynamicDescriptorSet::get()
{
	return *_descriptorSets[RenderContext::swapchain->getCurrentDynamicIndex()];
}