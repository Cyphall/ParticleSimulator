#pragma once

#include <ParticleSimulator/VKObject/VKObject.h>

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

class VKImageBase : public VKObject
{
public:
	vk::Image& getHandle();
	
	vk::ImageView& getView();
	
	vk::Format getFormat() const;
	
	const glm::uvec2& getSize() const;
	
	vk::ImageLayout getLayout() const;
	
	vk::ImageAspectFlags getAspect() const;
	
	int getMipLevels() const;
	
	vk::DeviceSize getByteSize() const;

protected:
	friend class VKCommandBuffer;
	
	VKImageBase(VKContext& context, vk::Format format, const glm::uvec2& size, vk::ImageLayout layout, int mipLevels, vk::ImageAspectFlags aspect);
	
	void setLayout(vk::ImageLayout layout);
	
	vk::Image _handle;
	vk::ImageView _view;
	
	vk::Format _format;
	glm::uvec2 _size;
	vk::ImageLayout _currentLayout;
	int _mipLevels;
	vk::ImageAspectFlags _aspect;
};