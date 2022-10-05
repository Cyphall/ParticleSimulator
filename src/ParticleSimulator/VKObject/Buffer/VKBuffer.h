#pragma once

#include <ParticleSimulator/VKObject/Buffer/VKBufferBase.h>
#include <ParticleSimulator/VKObject/VKContext.h>
#include <ParticleSimulator/VKObject/VKDynamic.h>

#include <stdexcept>
#include <vk_mem_alloc.hpp>

template<typename T>
class VKBuffer : public VKBufferBase
{
public:
	static VKPtr<VKBuffer<T>> create(
		VKContext& context,
		vk::BufferUsageFlags pipelineUsage,
		vk::MemoryPropertyFlags requiredProperties = {},
		vk::MemoryPropertyFlags preferredProperties = {})
	{
		return VKPtr<VKBuffer<T>>(new VKBuffer<T>(context, pipelineUsage, requiredProperties, preferredProperties));
	}
	
	static std::unique_ptr<VKDynamic<VKBuffer<T>>> createDynamic(
		VKContext& context,
		VKSwapchain& swapchain,
		vk::BufferUsageFlags pipelineUsage,
		vk::MemoryPropertyFlags requiredProperties = {},
		vk::MemoryPropertyFlags preferredProperties = {})
	{
		return std::unique_ptr<VKDynamic<VKBuffer<T>>>(new VKDynamic<VKBuffer<T>>(context, swapchain, pipelineUsage, requiredProperties, preferredProperties));
	}
	
	~VKBuffer() override
	{
		destroyBuffer();
	}
	
	void resize(uint64_t newSize)
	{
		if (newSize == _size)
		{
			return;
		}
		
		if (_size > 0)
		{
			destroyBuffer();
		}
		
		if (newSize > 0)
		{
			createBuffer(newSize);
		}
	}
	
	T* map()
	{
		if (_buffer)
		{
			return static_cast<T*>(_context.getVmaAllocator().mapMemory(_bufferAlloc));
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
			_context.getVmaAllocator().unmapMemory(_bufferAlloc);
		}
	}
	
	vk::Buffer& getHandle() override
	{
		return _buffer;
	}
	
	uint64_t getSize() const
	{
		return _size;
	}
	
	vk::DeviceSize getByteSize() override
	{
		return _size * sizeof(T);
	}

private:
	VKBuffer(
		VKContext& context,
		vk::BufferUsageFlags pipelineUsage,
		vk::MemoryPropertyFlags requiredProperties,
		vk::MemoryPropertyFlags preferredProperties):
		VKBufferBase(context), _pipelineUsage(pipelineUsage), _requiredProperties(requiredProperties), _preferredProperties(preferredProperties)
	{
		
	}
	
	void createBuffer(uint64_t size)
	{
		if (_buffer)
		{
			throw;
		}
		
		vk::BufferCreateInfo bufferCreateInfo;
		bufferCreateInfo.size = size * sizeof(T);
		bufferCreateInfo.usage = _pipelineUsage;
		bufferCreateInfo.sharingMode = vk::SharingMode::eExclusive;
		
		vma::AllocationCreateInfo allocationCreateInfo{};
		allocationCreateInfo.usage = vma::MemoryUsage::eUnknown;
		allocationCreateInfo.requiredFlags = _requiredProperties;
		allocationCreateInfo.preferredFlags = _preferredProperties;
		
		std::tie(_buffer, _bufferAlloc) = _context.getVmaAllocator().createBuffer(bufferCreateInfo, allocationCreateInfo);
		_size = size;
	}
	
	void destroyBuffer()
	{
		if (!_buffer)
		{
			throw;
		}
		
		_context.getVmaAllocator().destroyBuffer(_buffer, _bufferAlloc);
		_buffer = VK_NULL_HANDLE;
		_bufferAlloc = VK_NULL_HANDLE;
		_size = 0;
	}
	
	vk::Buffer _buffer;
	vma::Allocation _bufferAlloc;
	uint64_t _size = 0;
	
	vk::BufferUsageFlags _pipelineUsage;
	vk::MemoryPropertyFlags _requiredProperties;
	vk::MemoryPropertyFlags _preferredProperties;
};