#pragma once

#include <ParticleSimulator/VKObject/VKObject.h>

#include <vulkan/vulkan.hpp>

template<typename T>
class VKDynamic;
class VKSwapchain;
class VKDescriptorSetLayout;
class VKBufferBase;
class VKSampler;
class VKImageBase;

class VKDescriptorSet : public VKObject
{
public:
	static VKPtr<VKDescriptorSet> create(VKContext& context, const VKPtr<VKDescriptorSetLayout>& layout);
	static std::unique_ptr<VKDynamic<VKDescriptorSet>> createDynamic(VKContext& context, VKSwapchain& swapchain, const VKPtr<VKDescriptorSetLayout>& layout);
	
	~VKDescriptorSet() override;
	
	const VKPtr<VKDescriptorSetLayout>& getLayout();
	vk::DescriptorSet& getHandle();
	
	void bindBuffer(uint32_t binding, const VKPtr<VKBufferBase>& buffer, vk::DeviceSize offset = 0, vk::DeviceSize range = VK_WHOLE_SIZE, uint32_t arrayIndex = 0);
	void bindSampler(uint32_t binding, const VKPtr<VKSampler>& sampler, uint32_t arrayIndex = 0);
	void bindImage(uint32_t binding, const VKPtr<VKImageBase>& image, uint32_t arrayIndex = 0);
	
protected:
	VKDescriptorSet(VKContext& context, const VKPtr<VKDescriptorSetLayout>& layout);
	
	VKPtr<VKDescriptorSetLayout> _layout;
	vk::DescriptorPool _descriptorPool;
	vk::DescriptorSet _descriptorSet;
	
	std::unordered_map<uint32_t, std::vector<VKPtr<VKObject>>> _boundObjects;
};