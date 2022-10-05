#pragma once

#include <ParticleSimulator/VKObject/Image/VKImageBase.h>

class VKSwapchainImage : public VKImageBase
{
public:
	~VKSwapchainImage() override;
	
private:
	friend class VKSwapchain;
	
	VKSwapchainImage(VKContext& context, vk::Image handle, vk::Format format, const glm::uvec2& size);
};