#pragma once

#include <ParticleSimulator/VKObject/Buffer/VKBufferBase.h>
#include <ParticleSimulator/VKObject/VKContext.h>
#include <ParticleSimulator/VKObject/VKDynamic.h>

#include <stdexcept>
#include <vk_mem_alloc.h>

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
			void* data;
			vmaMapMemory(_context.getVmaAllocator(), _bufferAlloc, &data);
			return static_cast<T*>(data);
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
			vmaUnmapMemory(_context.getVmaAllocator(), _bufferAlloc);
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
		vk::MemoryPropertyFlags preferredProperties): VKBufferBase(context), _pipelineUsage(pipelineUsage), _requiredProperties(requiredProperties), _preferredProperties(preferredProperties) {}

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

		VmaAllocationCreateInfo allocationCreateInfo{};
		allocationCreateInfo.usage = VMA_MEMORY_USAGE_UNKNOWN;
		allocationCreateInfo.requiredFlags = static_cast<VkMemoryPropertyFlags>(_requiredProperties);
		allocationCreateInfo.preferredFlags = static_cast<VkMemoryPropertyFlags>(_preferredProperties);

		vmaCreateBuffer(
			_context.getVmaAllocator(),
			reinterpret_cast<VkBufferCreateInfo*>(&bufferCreateInfo),
			&allocationCreateInfo,
			reinterpret_cast<VkBuffer*>(&_buffer),
			&_bufferAlloc,
			nullptr
		);

		_size = size;
	}

	void destroyBuffer()
	{
		if (!_buffer)
		{
			throw;
		}

		vmaDestroyBuffer(_context.getVmaAllocator(), _buffer, _bufferAlloc);
		_buffer = VK_NULL_HANDLE;
		_bufferAlloc = VK_NULL_HANDLE;
		_size = 0;
	}

	vk::Buffer _buffer;
	VmaAllocation _bufferAlloc;
	uint64_t _size = 0;

	vk::BufferUsageFlags _pipelineUsage;
	vk::MemoryPropertyFlags _requiredProperties;
	vk::MemoryPropertyFlags _preferredProperties;
};