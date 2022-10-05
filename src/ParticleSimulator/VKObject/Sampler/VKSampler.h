#pragma once

#include <ParticleSimulator/VKObject/VKObject.h>

#include <vulkan/vulkan.hpp>

template<typename T>
class VKDynamic;
class VKSwapchain;

class VKSampler : public VKObject
{
public:
	static VKPtr<VKSampler> create(VKContext& context, const vk::SamplerCreateInfo& samplerCreateInfo);
	static std::unique_ptr<VKDynamic<VKSampler>> createDynamic(VKContext& context, VKSwapchain& swapchain, const vk::SamplerCreateInfo& samplerCreateInfo);
	
	~VKSampler() override;
	
	vk::Sampler& getHandle();

private:
	VKSampler(VKContext& context, const vk::SamplerCreateInfo& samplerCreateInfo);
	
	vk::Sampler _sampler;
};