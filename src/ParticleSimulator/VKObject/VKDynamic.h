#pragma once

#include <ParticleSimulator/VKObject/VKSwapchain.h>

#include <vector>
#include <memory>

template<typename T>
class VKDynamic
{
public:
	VKDynamic(const VKDynamic& other) = delete;
	VKDynamic& operator=(const VKDynamic& other) = delete;
	
	VKDynamic(VKDynamic&& other) = delete;
	VKDynamic& operator=(VKDynamic&& other) = delete;
	
	operator const VKPtr<T>&()
	{
		return _objects[_swapchain.getCurrentDynamicIndex()];
	}

private:
	template<typename>
	friend class VKBuffer;
	friend class VKCommandBuffer;
	friend class VKDescriptorSet;
	friend class VKDescriptorSetLayout;
	friend class VKFence;
	friend class VKImage;
	friend class VKComputePipeline;
	friend class VKGraphicsPipeline;
	friend class VKPipelineLayout;
	friend class VKSampler;
	friend class VKSemaphore;
	friend class VKShader;
	
	template<typename... Args>
	VKDynamic(VKContext& context, VKSwapchain& swapchain, Args&&... args):
		_swapchain(swapchain)
	{
		size_t count = _swapchain.getDynamicCount();
		_objects.reserve(count);
		for (size_t i = 0; i < count; i++)
		{
			_objects.push_back(T::create(context, std::forward<Args>(args)...));
		}
	}
	
	VKSwapchain& _swapchain;
	std::vector<VKPtr<T>> _objects;
};