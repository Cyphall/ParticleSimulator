#pragma once

#include <ParticleSimulator/VKObject/VKContext.h>
#include <ParticleSimulator/RenderContext.h>

#include <stdexcept>
#include <vk_mem_alloc.hpp>

template<typename T>
class VKBuffer
{
public:
	explicit VKBuffer(vk::BufferUsageFlags pipelineUsage, vk::MemoryPropertyFlags requiredProperties = {}, vk::MemoryPropertyFlags preferredProperties = {}):
		_pipelineUsage(pipelineUsage), _requiredProperties(requiredProperties), _preferredProperties(preferredProperties)
	{
		
	}
	
	~VKBuffer()
	{
		destroyBuffer();
	}
	
	void resize(uint64_t newSize)
	{
		if (newSize == _size)
		{
			return;
		}
		
		destroyBuffer();
		
		if (newSize > 0)
		{
			createBuffer(newSize);
		}
	}
	
	T* map()
	{
		if (_buffer)
		{
			return static_cast<T*>(RenderContext::vkContext->getVmaAllocator().mapMemory(_bufferAlloc));
		}
		else
		{
			return nullptr;
		}
	}
	
	void unmap()
	{
		if (_buffer)
		{
			RenderContext::vkContext->getVmaAllocator().unmapMemory(_bufferAlloc);
		}
	}
	
	vk::Buffer getHandle()
	{
		return _buffer;
	}
	
	uint64_t getSize() const
	{
		return _size;
	}
	
private:
	vk::Buffer _buffer;
	vma::Allocation _bufferAlloc;
	uint64_t _size = 0;
	
	vk::BufferUsageFlags _pipelineUsage;
	vk::MemoryPropertyFlags _requiredProperties;
	vk::MemoryPropertyFlags _preferredProperties;
	
	void createBuffer(uint64_t size)
	{
		vk::BufferCreateInfo bufferCreateInfo;
		bufferCreateInfo.size = size * sizeof(T);
		bufferCreateInfo.usage = _pipelineUsage;
		bufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;
		
		vma::AllocationCreateInfo allocationCreateInfo{};
		allocationCreateInfo.usage = vma::MemoryUsage::eUnknown;
		allocationCreateInfo.requiredFlags = _requiredProperties;
		allocationCreateInfo.preferredFlags = _preferredProperties;
		
		std::tie(_buffer, _bufferAlloc) = RenderContext::vkContext->getVmaAllocator().createBuffer(bufferCreateInfo, allocationCreateInfo);
		_size = size;
	}
	
	void destroyBuffer()
	{
		if (_size == 0)
		{
			return;
		}
		
		RenderContext::vkContext->getVmaAllocator().destroyBuffer(_buffer, _bufferAlloc);
		_buffer = VK_NULL_HANDLE;
		_bufferAlloc = VK_NULL_HANDLE;
		_size = 0;
	}
};