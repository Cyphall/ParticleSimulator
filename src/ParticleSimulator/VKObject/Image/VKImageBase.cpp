#include "VKImageBase.h"

#include <vulkan/vulkan_format_traits.hpp>

VKImageBase::VKImageBase(VKContext& context, vk::Format format, const glm::uvec2& size, vk::ImageLayout layout, int mipLevels, vk::ImageAspectFlags aspect):
	VKObject(context),
	_format(format),
	_size(size),
	_currentLayout(layout),
	_mipLevels(mipLevels),
	_aspect(aspect)
{
	
}

vk::Image& VKImageBase::getHandle()
{
	return _handle;
}

vk::ImageView& VKImageBase::getView()
{
	return _view;
}

vk::Format VKImageBase::getFormat() const
{
	return _format;
}

const glm::uvec2& VKImageBase::getSize() const
{
	return _size;
}

vk::ImageLayout VKImageBase::getLayout() const
{
	return _currentLayout;
}

vk::ImageAspectFlags VKImageBase::getAspect() const
{
	return _aspect;
}

int VKImageBase::getMipLevels() const
{
	return _mipLevels;
}

vk::DeviceSize VKImageBase::getByteSize() const
{
	uint8_t blockSize = vk::blockSize(_format);
	uint8_t texelsPerBlock = vk::texelsPerBlock(_format);
	
	return _size.x * _size.y * blockSize / texelsPerBlock;
}

void VKImageBase::setLayout(vk::ImageLayout layout)
{
	_currentLayout = layout;
}