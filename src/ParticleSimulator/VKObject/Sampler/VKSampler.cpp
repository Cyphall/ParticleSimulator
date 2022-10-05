#include "VKSampler.h"

#include <ParticleSimulator/VKObject/VKContext.h>
#include <ParticleSimulator/VKObject/VKDynamic.h>

VKPtr<VKSampler> VKSampler::create(VKContext& context, const vk::SamplerCreateInfo& samplerCreateInfo)
{
	return VKPtr<VKSampler>(new VKSampler(context, samplerCreateInfo));
}

std::unique_ptr<VKDynamic<VKSampler>> VKSampler::createDynamic(VKContext& context, VKSwapchain& swapchain, const vk::SamplerCreateInfo& samplerCreateInfo)
{
	return std::unique_ptr<VKDynamic<VKSampler>>(new VKDynamic<VKSampler>(context, swapchain, samplerCreateInfo));
}

VKSampler::VKSampler(VKContext& context, const vk::SamplerCreateInfo& samplerCreateInfo):
	VKObject(context)
{
	_sampler = _context.getDevice().createSampler(samplerCreateInfo);
}

VKSampler::~VKSampler()
{
	_context.getDevice().destroySampler(_sampler);
}

vk::Sampler& VKSampler::getHandle()
{
	return _sampler;
}