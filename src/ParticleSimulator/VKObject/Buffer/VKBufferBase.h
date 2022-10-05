#pragma once

#include <vulkan/vulkan.hpp>
#include <ParticleSimulator/VKObject/VKObject.h>

class VKBufferBase : public VKObject
{
public:
	virtual vk::Buffer& getHandle() = 0;
	
	virtual vk::DeviceSize getByteSize() = 0;
	
protected:
	explicit VKBufferBase(VKContext& context):
		VKObject(context)
	{
		
	}
};