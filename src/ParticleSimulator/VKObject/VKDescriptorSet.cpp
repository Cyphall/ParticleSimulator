#include "VKDescriptorSet.h"

#include <ParticleSimulator/RenderContext.h>
#include <ParticleSimulator/VKObject/VKSwapchain.h>
#include <ParticleSimulator/VKObject/VKContext.h>
#include <ParticleSimulator/VKObject/VKDescriptorSetLayout.h>

VKDescriptorSet::VKDescriptorSet(VKDescriptorSetLayout& layout):
	_layout(layout)
{
	std::vector<vk::DescriptorPoolSize> requiredResources;
	for (const auto& [binding, bindingInfo] : _layout.getBindingInfos())
	{
		vk::DescriptorPoolSize& descriptorPoolSize = requiredResources.emplace_back();
		descriptorPoolSize.type = bindingInfo.type;
		descriptorPoolSize.descriptorCount = bindingInfo.count;
	}
	
	vk::DescriptorPoolCreateInfo poolInfo;
	poolInfo.poolSizeCount = requiredResources.size();
	poolInfo.pPoolSizes = requiredResources.data();
	poolInfo.maxSets = 1;
	
	_descriptorPool = RenderContext::vkContext->getDevice().createDescriptorPool(poolInfo);
	
	vk::DescriptorSetLayout vkLayout = _layout.getHandle();
	
	vk::DescriptorSetAllocateInfo allocInfo;
	allocInfo.descriptorPool = _descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &vkLayout;
	
	_descriptorSet = RenderContext::vkContext->getDevice().allocateDescriptorSets(allocInfo).front();
}

VKDescriptorSet::~VKDescriptorSet()
{
	RenderContext::vkContext->getDevice().destroyDescriptorPool(_descriptorPool);
}

VKDescriptorSetLayout& VKDescriptorSet::getLayout()
{
	return _layout;
}

vk::DescriptorSet VKDescriptorSet::getHandle()
{
	return _descriptorSet;
}

void VKDescriptorSet::bindBuffer(uint32_t binding, vk::Buffer buffer, vk::DeviceSize offset, vk::DeviceSize range, uint32_t arrayIndex)
{
	vk::DescriptorBufferInfo bufferInfo;
	bufferInfo.buffer = buffer;
	bufferInfo.offset = offset;
	bufferInfo.range = range;
	
	const VKDescriptorSetLayoutBinding& bindingInfo = _layout.getBindingInfo(binding);
	
	vk::WriteDescriptorSet descriptorWrite;
	descriptorWrite.dstSet = _descriptorSet;
	descriptorWrite.dstBinding = binding;
	descriptorWrite.dstArrayElement = arrayIndex;
	descriptorWrite.descriptorType = bindingInfo.type;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pImageInfo = nullptr; // Optional
	descriptorWrite.pBufferInfo = &bufferInfo;
	descriptorWrite.pTexelBufferView = nullptr; // Optional
	
	RenderContext::vkContext->getDevice().updateDescriptorSets(descriptorWrite, nullptr);
}

void VKDescriptorSet::bindSampler(uint32_t binding, vk::Sampler sampler, uint32_t arrayIndex)
{
	vk::DescriptorImageInfo samplerInfo;
	samplerInfo.sampler = sampler;
	
	const VKDescriptorSetLayoutBinding& bindingInfo = _layout.getBindingInfo(binding);
	
	vk::WriteDescriptorSet descriptorWrite;
	descriptorWrite.dstSet = _descriptorSet;
	descriptorWrite.dstBinding = binding;
	descriptorWrite.dstArrayElement = arrayIndex;
	descriptorWrite.descriptorType = bindingInfo.type;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pImageInfo = &samplerInfo;
	descriptorWrite.pBufferInfo = nullptr;
	descriptorWrite.pTexelBufferView = nullptr;
	
	RenderContext::vkContext->getDevice().updateDescriptorSets(descriptorWrite, nullptr);
}

void VKDescriptorSet::bindImage(uint32_t binding, vk::ImageView imageView, vk::ImageLayout imageLayout, uint32_t arrayIndex)
{
	vk::DescriptorImageInfo imageInfo;
	imageInfo.imageView = imageView;
	imageInfo.imageLayout = imageLayout;
	
	const VKDescriptorSetLayoutBinding& bindingInfo = _layout.getBindingInfo(binding);
	
	vk::WriteDescriptorSet descriptorWrite;
	descriptorWrite.dstSet = _descriptorSet;
	descriptorWrite.dstBinding = binding;
	descriptorWrite.dstArrayElement = arrayIndex;
	descriptorWrite.descriptorType = bindingInfo.type;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pImageInfo = &imageInfo;
	descriptorWrite.pBufferInfo = nullptr;
	descriptorWrite.pTexelBufferView = nullptr;
	
	RenderContext::vkContext->getDevice().updateDescriptorSets(descriptorWrite, nullptr);
}