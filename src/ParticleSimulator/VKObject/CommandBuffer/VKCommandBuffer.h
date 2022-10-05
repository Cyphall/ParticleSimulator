#pragma once

#include <ParticleSimulator/VKObject/VKObject.h>

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

template<typename T>
class VKDynamic;
class VKSwapchain;
class VKBufferBase;
class VKImageBase;
class VKDescriptorSet;
class VKPipeline;
class VKFence;

class VKCommandBuffer : public VKObject
{
public:
	static VKPtr<VKCommandBuffer> create(VKContext& context);
	static std::unique_ptr<VKDynamic<VKCommandBuffer>> createDynamic(VKContext& context, VKSwapchain& swapchain);
	
	~VKCommandBuffer() override;
	
	vk::CommandBuffer& getHandle();
	
	bool waitExecution() const;
	
	void begin();
	void end();
	void reset();
	
	void bufferMemoryBarrier(const VKPtr<VKBufferBase>& buffer, vk::PipelineStageFlags2 srcStageMask, vk::AccessFlags2 srcAccessMask, vk::PipelineStageFlags2 dstStageMask, vk::AccessFlags2 dstAccessMask);
	void imageMemoryBarrier(const VKPtr<VKImageBase>& image, vk::PipelineStageFlags2 srcStageMask, vk::AccessFlags2 srcAccessMask, vk::PipelineStageFlags2 dstStageMask, vk::AccessFlags2 dstAccessMask);
	void imageMemoryBarrier(const VKPtr<VKImageBase>& image, vk::PipelineStageFlags2 srcStageMask, vk::AccessFlags2 srcAccessMask, vk::PipelineStageFlags2 dstStageMask, vk::AccessFlags2 dstAccessMask, vk::ImageLayout newImageLayout);
	
	void beginRendering(const vk::RenderingInfo& renderingInfo);
	void endRendering();
	
	void bindPipeline(const VKPtr<VKPipeline>& pipeline);
	void unbindPipeline();
	
	void bindDescriptorSet(uint32_t descriptorSetIndex, const VKPtr<VKDescriptorSet>& descriptorSet);
	void bindDescriptorSet(uint32_t descriptorSetIndex, const VKPtr<VKDescriptorSet>& descriptorSet, uint32_t dynamicOffset);
	
	template<typename T>
	void pushConstants(vk::ShaderStageFlags shaderStages, const T& data)
	{
		pushConstants(shaderStages, &data, sizeof(T));
	}
	
	void bindVertexBuffer(uint32_t vertexBufferIndex, const VKPtr<VKBufferBase>& vertexBuffer);
	
	void draw(uint32_t vertexCount, uint32_t offset);
	
	void copyBufferToImage(const VKPtr<VKBufferBase>& srcBuffer, const VKPtr<VKImageBase>& dstImage);
	void copyBufferToImage(const VKPtr<VKBufferBase>& srcBuffer, vk::DeviceSize srcOffset, const VKPtr<VKImageBase>& dstImage, const glm::uvec2& dstOffset, uint32_t dstMipLevel, const glm::uvec2& size);
	
	void copyBufferToBuffer(const VKPtr<VKBufferBase>& srcBuffer, const VKPtr<VKBufferBase>& dstBuffer);
	void copyBufferToBuffer(const VKPtr<VKBufferBase>& srcBuffer, vk::DeviceSize srcOffset, const VKPtr<VKBufferBase>& dstBuffer, vk::DeviceSize dstOffset, vk::DeviceSize size);
	
	void copyImageToBuffer(const VKPtr<VKImageBase>& srcImage, const VKPtr<VKBufferBase>& dstBuffer);
	void copyImageToBuffer(const VKPtr<VKImageBase>& srcImage, const glm::uvec2& srcOffset, uint32_t srcMipLevel, const VKPtr<VKBufferBase>& dstBuffer, vk::DeviceSize dstOffset, const glm::uvec2& size);
	
	void copyImageToImage(const VKPtr<VKImageBase>& srcImage, const VKPtr<VKImageBase>& dstImage);
	void copyImageToImage(const VKPtr<VKImageBase>& srcImage, const glm::uvec2& srcOffset, uint32_t srcMipLevel, const VKPtr<VKImageBase>& dstImage, const glm::uvec2& dstOffset, uint32_t dstMipLevel, const glm::uvec2& size);
	
	void dispatch(glm::uvec3 groupCount);
	
private:
	friend class VKQueue;
	
	explicit VKCommandBuffer(VKContext& context);
	
	void pushConstants(vk::ShaderStageFlags shaderStages, const void* data, uint32_t dataSize);
	
	const VKPtr<VKFence>& getStatusFence();
	
	vk::CommandPool _commandPool;
	vk::CommandBuffer _commandBuffer;
	
	VKPtr<VKFence> _statusFence;
	
	VKPipeline* _boundPipeline = nullptr;
	
	std::vector<VKPtr<VKObject>> _usedObjects;
};