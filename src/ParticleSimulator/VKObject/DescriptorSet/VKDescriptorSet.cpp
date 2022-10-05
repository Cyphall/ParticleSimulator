#include "VKDescriptorSet.h"

#include <ParticleSimulator/VKObject/VKContext.h>
#include <ParticleSimulator/VKObject/VKDynamic.h>
#include <ParticleSimulator/VKObject/DescriptorSet/VKDescriptorSetLayout.h>
#include <ParticleSimulator/VKObject/Buffer/VKBufferBase.h>
#include <ParticleSimulator/VKObject/Sampler/VKSampler.h>
#include <ParticleSimulator/VKObject/Image/VKImageBase.h>

VKPtr<VKDescriptorSet> VKDescriptorSet::create(VKContext& context, const VKPtr<VKDescriptorSetLayout>& layout)
{
	return VKPtr<VKDescriptorSet>(new VKDescriptorSet(context, layout));
}

std::unique_ptr<VKDynamic<VKDescriptorSet>> VKDescriptorSet::createDynamic(VKContext& context, VKSwapchain& swapchain, const VKPtr<VKDescriptorSetLayout>& layout)
{
	return std::unique_ptr<VKDynamic<VKDescriptorSet>>(new VKDynamic<VKDescriptorSet>(context, swapchain, layout));
}

VKDescriptorSet::VKDescriptorSet(VKContext& context, const VKPtr<VKDescriptorSetLayout>& layout):
	VKObject(context), _layout(layout)
{
	std::vector<vk::DescriptorPoolSize> requiredResources;
	for (const auto& [binding, bindingInfo] : _layout->getBindingInfos())
	{
		vk::DescriptorPoolSize& descriptorPoolSize = requiredResources.emplace_back();
		descriptorPoolSize.type = bindingInfo.type;
		descriptorPoolSize.descriptorCount = bindingInfo.count;
		
		auto [it, inserted] = _boundObjects.try_emplace(binding);
		it->second.resize(bindingInfo.count);
	}
	
	vk::DescriptorPoolCreateInfo poolInfo;
	poolInfo.poolSizeCount = requiredResources.size();
	poolInfo.pPoolSizes = requiredResources.data();
	poolInfo.maxSets = 1;
	
	_descriptorPool = _context.getDevice().createDescriptorPool(poolInfo);
	
	vk::DescriptorSetLayout vkLayout = _layout->getHandle();
	
	vk::DescriptorSetAllocateInfo allocInfo;
	allocInfo.descriptorPool = _descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &vkLayout;
	
	_descriptorSet = _context.getDevice().allocateDescriptorSets(allocInfo).front();
}

VKDescriptorSet::~VKDescriptorSet()
{
	_context.getDevice().destroyDescriptorPool(_descriptorPool);
}

const VKPtr<VKDescriptorSetLayout>& VKDescriptorSet::getLayout()
{
	return _layout;
}

vk::DescriptorSet& VKDescriptorSet::getHandle()
{
	return _descriptorSet;
}

void VKDescriptorSet::bindBuffer(uint32_t binding, const VKPtr<VKBufferBase>& buffer, vk::DeviceSize offset, vk::DeviceSize range, uint32_t arrayIndex)
{
	vk::DescriptorBufferInfo bufferInfo;
	bufferInfo.buffer = buffer->getHandle();
	bufferInfo.offset = offset;
	bufferInfo.range = range;
	
	const VKDescriptorSetLayoutBinding& bindingInfo = _layout->getBindingInfo(binding);
	
	vk::WriteDescriptorSet descriptorWrite;
	descriptorWrite.dstSet = _descriptorSet;
	descriptorWrite.dstBinding = binding;
	descriptorWrite.dstArrayElement = arrayIndex;
	descriptorWrite.descriptorType = bindingInfo.type;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pImageInfo = nullptr; // Optional
	descriptorWrite.pBufferInfo = &bufferInfo;
	descriptorWrite.pTexelBufferView = nullptr; // Optional
	
	_context.getDevice().updateDescriptorSets(descriptorWrite, nullptr);
	
	_boundObjects.at(binding).at(arrayIndex) = buffer;
}

void VKDescriptorSet::bindSampler(uint32_t binding, const VKPtr<VKSampler>& sampler, uint32_t arrayIndex)
{
	vk::DescriptorImageInfo samplerInfo;
	samplerInfo.sampler = sampler->getHandle();
	
	const VKDescriptorSetLayoutBinding& bindingInfo = _layout->getBindingInfo(binding);
	
	vk::WriteDescriptorSet descriptorWrite;
	descriptorWrite.dstSet = _descriptorSet;
	descriptorWrite.dstBinding = binding;
	descriptorWrite.dstArrayElement = arrayIndex;
	descriptorWrite.descriptorType = bindingInfo.type;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pImageInfo = &samplerInfo;
	descriptorWrite.pBufferInfo = nullptr;
	descriptorWrite.pTexelBufferView = nullptr;
	
	_context.getDevice().updateDescriptorSets(descriptorWrite, nullptr);
	
	_boundObjects.at(binding).at(arrayIndex) = sampler;
}

void VKDescriptorSet::bindImage(uint32_t binding, const VKPtr<VKImageBase>& image, uint32_t arrayIndex)
{
	vk::DescriptorImageInfo imageInfo;
	imageInfo.imageView = image->getView();
	imageInfo.imageLayout = image->getLayout();
	
	const VKDescriptorSetLayoutBinding& bindingInfo = _layout->getBindingInfo(binding);
	
	vk::WriteDescriptorSet descriptorWrite;
	descriptorWrite.dstSet = _descriptorSet;
	descriptorWrite.dstBinding = binding;
	descriptorWrite.dstArrayElement = arrayIndex;
	descriptorWrite.descriptorType = bindingInfo.type;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pImageInfo = &imageInfo;
	descriptorWrite.pBufferInfo = nullptr;
	descriptorWrite.pTexelBufferView = nullptr;
	
	_context.getDevice().updateDescriptorSets(descriptorWrite, nullptr);
	
	_boundObjects.at(binding).at(arrayIndex) = image;
}