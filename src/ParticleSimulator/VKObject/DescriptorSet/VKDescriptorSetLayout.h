#pragma once

#include <ParticleSimulator/VKObject/VKObject.h>
#include <ParticleSimulator/VKObject/DescriptorSet/VKDescriptorSetLayoutBinding.h>

#include <vulkan/vulkan.hpp>
#include <unordered_map>

template<typename T>
class VKDynamic;
class VKSwapchain;
class VKDescriptorSetLayoutInfo;

class VKDescriptorSetLayout : public VKObject
{
public:
	static VKPtr<VKDescriptorSetLayout> create(VKContext& context, const VKDescriptorSetLayoutInfo& descriptorSetLayoutInfo);
	static std::unique_ptr<VKDynamic<VKDescriptorSetLayout>> createDynamic(VKContext& context, VKSwapchain& swapchain, const VKDescriptorSetLayoutInfo& descriptorSetLayoutInfo);
	
	~VKDescriptorSetLayout() override;
	
	vk::DescriptorSetLayout& getHandle();
	const VKDescriptorSetLayoutBinding& getBindingInfo(uint32_t binding) const;
	const std::unordered_map<uint32_t, VKDescriptorSetLayoutBinding>& getBindingInfos() const;

private:
	VKDescriptorSetLayout(VKContext& context, const VKDescriptorSetLayoutInfo& descriptorSetLayoutInfo);
	
	std::unordered_map<uint32_t, VKDescriptorSetLayoutBinding> _bindingsInfo;
	vk::DescriptorSetLayout _handle;
};