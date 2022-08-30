#pragma once

#include "VKImageBase.h"

class VKSwapchainImage : public VKImageBase
{
public:
	VKSwapchainImage(vk::Image handle, vk::Format format, vk::Extent2D extent);
	~VKSwapchainImage() override;
};