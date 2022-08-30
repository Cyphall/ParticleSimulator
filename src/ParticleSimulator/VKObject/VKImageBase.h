#pragma once

#include <vulkan/vulkan.hpp>

class VKImageBase
{
public:
	VKImageBase(vk::Format format, vk::Extent2D extent, vk::ImageLayout layout, int mipLevels, vk::ImageAspectFlags aspect);
	virtual ~VKImageBase() = default;
	
	VKImageBase(const VKImageBase& other) = delete;
	VKImageBase& operator=(const VKImageBase& other) = delete;
	
	VKImageBase(VKImageBase&& other) = delete;
	VKImageBase& operator=(VKImageBase&& other) = delete;
	
	void transitionLayout(vk::CommandBuffer commandBuffer, vk::ImageLayout newLayout);
	
	vk::Image getHandle();
	
	vk::ImageView getView();
	
	vk::Format getFormat() const;
	
	vk::Extent2D getExtent() const;
	
	vk::ImageLayout getLayout() const;

protected:
	vk::Image _handle;
	vk::ImageView _view;
	
	vk::Format _format;
	vk::Extent2D _extent;
	vk::ImageLayout _currentLayout;
	int _mipLevels;
	vk::ImageAspectFlags _aspect;
};