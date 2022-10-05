#pragma once

#include <ParticleSimulator/VKObject/VKObject.h>

#include <vulkan/vulkan.hpp>

template<typename T>
class VKDynamic;
class VKSwapchain;

class VKFence : public VKObject
{
public:
	static VKPtr<VKFence> create(VKContext& context, const vk::FenceCreateInfo& fenceCreateInfo);
	static std::unique_ptr<VKDynamic<VKFence>> createDynamic(VKContext& context, VKSwapchain& swapchain, const vk::FenceCreateInfo& fenceCreateInfo);
	
	~VKFence() override;
	
	bool wait(uint64_t timeout = UINT64_MAX) const;
	bool isSignaled() const;
	
	void reset();
	
	vk::Fence& getHandle();

private:
	VKFence(VKContext& context, const vk::FenceCreateInfo& fenceCreateInfo);
	
	vk::Fence _fence;
};