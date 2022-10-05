#include "VKCommandBuffer.h"

#include <ParticleSimulator/VKObject/VKContext.h>
#include <ParticleSimulator/VKObject/VKDynamic.h>
#include <ParticleSimulator/VKObject/Buffer/VKBufferBase.h>
#include <ParticleSimulator/VKObject/Image/VKImageBase.h>
#include <ParticleSimulator/VKObject/DescriptorSet/VKDescriptorSet.h>
#include <ParticleSimulator/VKObject/Pipeline/VKPipeline.h>
#include <ParticleSimulator/VKObject/Pipeline/VKPipelineLayout.h>
#include <ParticleSimulator/VKObject/Queue/VKQueue.h>
#include <ParticleSimulator/VKObject/Fence/VKFence.h>

VKPtr<VKCommandBuffer> VKCommandBuffer::create(VKContext& context)
{
	return VKPtr<VKCommandBuffer>(new VKCommandBuffer(context));
}

std::unique_ptr<VKDynamic<VKCommandBuffer>> VKCommandBuffer::createDynamic(VKContext& context, VKSwapchain& swapchain)
{
	return std::unique_ptr<VKDynamic<VKCommandBuffer>>(new VKDynamic<VKCommandBuffer>(context, swapchain));
}

VKCommandBuffer::VKCommandBuffer(VKContext& context):
	VKObject(context)
{
	vk::CommandPoolCreateInfo poolCreateInfo;
	poolCreateInfo.queueFamilyIndex = _context.getQueue().getFamily();
	
	_commandPool = _context.getDevice().createCommandPool(poolCreateInfo);
	
	vk::CommandBufferAllocateInfo allocateInfo;
	allocateInfo.commandPool = _commandPool;
	allocateInfo.level = vk::CommandBufferLevel::ePrimary;
	allocateInfo.commandBufferCount = 1;
	
	_commandBuffer = _context.getDevice().allocateCommandBuffers(allocateInfo).front();
	
	vk::FenceCreateInfo fenceCreateInfo;
	fenceCreateInfo.flags = vk::FenceCreateFlagBits::eSignaled;
	
	_statusFence = VKFence::create(_context, fenceCreateInfo);
}

VKCommandBuffer::~VKCommandBuffer()
{
	_context.getDevice().destroyCommandPool(_commandPool);
}

vk::CommandBuffer& VKCommandBuffer::getHandle()
{
	return _commandBuffer;
}

bool VKCommandBuffer::waitExecution() const
{
	return _statusFence->wait();
}

void VKCommandBuffer::begin()
{
	waitExecution();
	
	reset();
	
	vk::CommandBufferBeginInfo commandBufferBeginInfo;
	commandBufferBeginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
	commandBufferBeginInfo.pInheritanceInfo = nullptr; // Optional
	
	_commandBuffer.begin(commandBufferBeginInfo);
}

void VKCommandBuffer::end()
{
	_commandBuffer.end();
}

void VKCommandBuffer::reset()
{
	_context.getDevice().resetCommandPool(_commandPool);
	_usedObjects.clear();
}

void VKCommandBuffer::bufferMemoryBarrier(const VKPtr<VKBufferBase>& buffer, vk::PipelineStageFlags2 srcStageMask, vk::AccessFlags2 srcAccessMask, vk::PipelineStageFlags2 dstStageMask, vk::AccessFlags2 dstAccessMask)
{
	vk::BufferMemoryBarrier2 bufferMemoryBarrier;
	bufferMemoryBarrier.srcStageMask = srcStageMask;
	bufferMemoryBarrier.srcAccessMask = srcAccessMask;
	bufferMemoryBarrier.dstStageMask = dstStageMask;
	bufferMemoryBarrier.dstAccessMask = dstAccessMask;
	bufferMemoryBarrier.buffer = buffer->getHandle();
	bufferMemoryBarrier.offset = 0;
	bufferMemoryBarrier.size = VK_WHOLE_SIZE;
	
	vk::DependencyInfo dependencyInfo;
	dependencyInfo.bufferMemoryBarrierCount = 1;
	dependencyInfo.pBufferMemoryBarriers = &bufferMemoryBarrier;
	
	_commandBuffer.pipelineBarrier2(dependencyInfo);
	
	_usedObjects.emplace_back(buffer);
}

void VKCommandBuffer::imageMemoryBarrier(const VKPtr<VKImageBase>& image, vk::PipelineStageFlags2 srcStageMask, vk::AccessFlags2 srcAccessMask, vk::PipelineStageFlags2 dstStageMask, vk::AccessFlags2 dstAccessMask)
{
	vk::ImageMemoryBarrier2 imageMemoryBarrier;
	imageMemoryBarrier.srcStageMask = srcStageMask;
	imageMemoryBarrier.srcAccessMask = srcAccessMask;
	imageMemoryBarrier.dstStageMask = dstStageMask;
	imageMemoryBarrier.dstAccessMask = dstAccessMask;
	imageMemoryBarrier.oldLayout = image->getLayout();
	imageMemoryBarrier.newLayout = image->getLayout();
	imageMemoryBarrier.image = image->getHandle();
	imageMemoryBarrier.subresourceRange.aspectMask = image->getAspect();
	imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	imageMemoryBarrier.subresourceRange.levelCount = image->getMipLevels();
	imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	imageMemoryBarrier.subresourceRange.layerCount = 1;
	
	vk::DependencyInfo dependencyInfo;
	dependencyInfo.imageMemoryBarrierCount = 1;
	dependencyInfo.pImageMemoryBarriers = &imageMemoryBarrier;
	
	_commandBuffer.pipelineBarrier2(dependencyInfo);
	
	_usedObjects.emplace_back(image);
}

void VKCommandBuffer::imageMemoryBarrier(const VKPtr<VKImageBase>& image, vk::PipelineStageFlags2 srcStageMask, vk::AccessFlags2 srcAccessMask, vk::PipelineStageFlags2 dstStageMask, vk::AccessFlags2 dstAccessMask, vk::ImageLayout newImageLayout)
{
	vk::ImageMemoryBarrier2 imageMemoryBarrier;
	imageMemoryBarrier.srcStageMask = srcStageMask;
	imageMemoryBarrier.srcAccessMask = srcAccessMask;
	imageMemoryBarrier.dstStageMask = dstStageMask;
	imageMemoryBarrier.dstAccessMask = dstAccessMask;
	imageMemoryBarrier.oldLayout = image->getLayout();
	imageMemoryBarrier.newLayout = newImageLayout;
	imageMemoryBarrier.image = image->getHandle();
	imageMemoryBarrier.subresourceRange.aspectMask = image->getAspect();
	imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	imageMemoryBarrier.subresourceRange.levelCount = image->getMipLevels();
	imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	imageMemoryBarrier.subresourceRange.layerCount = 1;
	
	vk::DependencyInfo dependencyInfo;
	dependencyInfo.imageMemoryBarrierCount = 1;
	dependencyInfo.pImageMemoryBarriers = &imageMemoryBarrier;
	
	_commandBuffer.pipelineBarrier2(dependencyInfo);
	
	image->setLayout(newImageLayout);
	
	_usedObjects.emplace_back(image);
}

void VKCommandBuffer::beginRendering(const vk::RenderingInfo& renderingInfo)
{
	if (_boundPipeline != nullptr)
		throw;
	
	_commandBuffer.beginRendering(renderingInfo);
}

void VKCommandBuffer::endRendering()
{
	if (_boundPipeline != nullptr)
		throw;
	
	_commandBuffer.endRendering();
}

void VKCommandBuffer::bindPipeline(const VKPtr<VKPipeline>& pipeline)
{
	if (_boundPipeline != nullptr)
		throw;
	
	_commandBuffer.bindPipeline(pipeline->getPipelineType(), pipeline->getHandle());
	_boundPipeline = pipeline.get();
	
	_usedObjects.emplace_back(pipeline);
}

void VKCommandBuffer::unbindPipeline()
{
	if (_boundPipeline == nullptr)
		throw;
	
	_boundPipeline = nullptr;
}

void VKCommandBuffer::bindDescriptorSet(uint32_t descriptorSetIndex, const VKPtr<VKDescriptorSet>& descriptorSet)
{
	if (_boundPipeline == nullptr)
		throw;
	
	_commandBuffer.bindDescriptorSets(
		_boundPipeline->getPipelineType(),
		_boundPipeline->getPipelineLayout()->getHandle(),
		descriptorSetIndex,
		descriptorSet->getHandle(),
		{});
	
	_usedObjects.emplace_back(descriptorSet);
}

void VKCommandBuffer::bindDescriptorSet(uint32_t descriptorSetIndex, const VKPtr<VKDescriptorSet>& descriptorSet, uint32_t dynamicOffset)
{
	if (_boundPipeline == nullptr)
		throw;
	
	_commandBuffer.bindDescriptorSets(
		_boundPipeline->getPipelineType(),
		_boundPipeline->getPipelineLayout()->getHandle(),
		descriptorSetIndex,
		descriptorSet->getHandle(),
		dynamicOffset);
	
	_usedObjects.emplace_back(descriptorSet);
}

void VKCommandBuffer::bindVertexBuffer(uint32_t vertexBufferIndex, const VKPtr<VKBufferBase>& vertexBuffer)
{
	if (_boundPipeline == nullptr)
		throw;
	
	_commandBuffer.bindVertexBuffers(vertexBufferIndex, vertexBuffer->getHandle(), {0});
	
	_usedObjects.emplace_back(vertexBuffer);
}

void VKCommandBuffer::draw(uint32_t vertexCount, uint32_t offset)
{
	_commandBuffer.draw(vertexCount, 1, offset, 0);
}

void VKCommandBuffer::copyBufferToImage(const VKPtr<VKBufferBase>& srcBuffer, const VKPtr<VKImageBase>& dstImage)
{
	if (srcBuffer->getByteSize() != dstImage->getByteSize())
		throw;
	
	copyBufferToImage(srcBuffer, 0, dstImage, {0, 0}, 0, dstImage->getSize());
}

void VKCommandBuffer::copyBufferToImage(const VKPtr<VKBufferBase>& srcBuffer, vk::DeviceSize srcOffset, const VKPtr<VKImageBase>& dstImage, const glm::uvec2& dstOffset, uint32_t dstMipLevel, const glm::uvec2& size)
{
	vk::BufferImageCopy2 copiedRegion;
	copiedRegion.bufferOffset = srcOffset;
	copiedRegion.bufferRowLength = 0;
	copiedRegion.bufferImageHeight = 0;
	copiedRegion.imageSubresource.aspectMask = dstImage->getAspect();
	copiedRegion.imageSubresource.mipLevel = dstMipLevel;
	copiedRegion.imageSubresource.baseArrayLayer = 0;
	copiedRegion.imageSubresource.layerCount = 1;
	copiedRegion.imageOffset = vk::Offset3D(dstOffset.x, dstOffset.y, 0);
	copiedRegion.imageExtent = vk::Extent3D(size.x, size.y, 1);
	
	vk::CopyBufferToImageInfo2 copyInfo;
	copyInfo.srcBuffer = srcBuffer->getHandle();
	copyInfo.dstImage = dstImage->getHandle();
	copyInfo.dstImageLayout = dstImage->getLayout();
	copyInfo.regionCount = 1;
	copyInfo.pRegions = &copiedRegion;
	
	_commandBuffer.copyBufferToImage2(copyInfo);
	
	_usedObjects.emplace_back(srcBuffer);
	_usedObjects.emplace_back(dstImage);
}

void VKCommandBuffer::copyBufferToBuffer(const VKPtr<VKBufferBase>& srcBuffer, const VKPtr<VKBufferBase>& dstBuffer)
{
	if (srcBuffer->getByteSize() != dstBuffer->getByteSize())
		throw;
	
	copyBufferToBuffer(srcBuffer, 0, dstBuffer, 0, srcBuffer->getByteSize());
}

void VKCommandBuffer::copyBufferToBuffer(const VKPtr<VKBufferBase>& srcBuffer, vk::DeviceSize srcOffset, const VKPtr<VKBufferBase>& dstBuffer, vk::DeviceSize dstOffset, vk::DeviceSize size)
{
	vk::BufferCopy2 copiedRegion;
	copiedRegion.srcOffset = srcOffset;
	copiedRegion.dstOffset = dstOffset;
	copiedRegion.size = size;
	
	vk::CopyBufferInfo2 copyInfo;
	copyInfo.srcBuffer = srcBuffer->getHandle();
	copyInfo.dstBuffer = dstBuffer->getHandle();
	copyInfo.regionCount = 1;
	copyInfo.pRegions = &copiedRegion;
	
	_commandBuffer.copyBuffer2(copyInfo);
	
	_usedObjects.emplace_back(srcBuffer);
	_usedObjects.emplace_back(dstBuffer);
}

void VKCommandBuffer::copyImageToBuffer(const VKPtr<VKImageBase>& srcImage, const VKPtr<VKBufferBase>& dstBuffer)
{
	if (srcImage->getByteSize() != dstBuffer->getByteSize())
		throw;
	
	copyImageToBuffer(srcImage, {0, 0}, 0, dstBuffer, 0, srcImage->getSize());
}

void VKCommandBuffer::copyImageToBuffer(const VKPtr<VKImageBase>& srcImage, const glm::uvec2& srcOffset, uint32_t srcMipLevel, const VKPtr<VKBufferBase>& dstBuffer, vk::DeviceSize dstOffset, const glm::uvec2& size)
{
	vk::BufferImageCopy2 copiedRegion;
	copiedRegion.bufferOffset = dstOffset;
	copiedRegion.bufferRowLength = 0;
	copiedRegion.bufferImageHeight = 0;
	copiedRegion.imageSubresource.aspectMask = srcImage->getAspect();
	copiedRegion.imageSubresource.mipLevel = srcMipLevel;
	copiedRegion.imageSubresource.baseArrayLayer = 0;
	copiedRegion.imageSubresource.layerCount = 1;
	copiedRegion.imageOffset = vk::Offset3D(srcOffset.x, srcOffset.y, 0);
	copiedRegion.imageExtent = vk::Extent3D(size.x, size.y, 1);
	
	vk::CopyImageToBufferInfo2 copyInfo;
	copyInfo.srcImage = srcImage->getHandle();
	copyInfo.srcImageLayout = srcImage->getLayout();
	copyInfo.dstBuffer = dstBuffer->getHandle();
	copyInfo.regionCount = 1;
	copyInfo.pRegions = &copiedRegion;
	
	_commandBuffer.copyImageToBuffer2(copyInfo);
	
	_usedObjects.emplace_back(srcImage);
	_usedObjects.emplace_back(dstBuffer);
}

void VKCommandBuffer::copyImageToImage(const VKPtr<VKImageBase>& srcImage, const VKPtr<VKImageBase>& dstImage)
{
	if (srcImage->getByteSize() != dstImage->getByteSize())
		throw;
	
	copyImageToImage(srcImage, {0, 0}, 0, dstImage, {0, 0}, 0, srcImage->getSize());
}

void VKCommandBuffer::copyImageToImage(const VKPtr<VKImageBase>& srcImage, const glm::uvec2& srcOffset, uint32_t srcMipLevel, const VKPtr<VKImageBase>& dstImage, const glm::uvec2& dstOffset, uint32_t dstMipLevel, const glm::uvec2& size)
{
	vk::ImageCopy2 copiedRegion;
	copiedRegion.srcSubresource.aspectMask = srcImage->getAspect();
	copiedRegion.srcSubresource.mipLevel = srcMipLevel;
	copiedRegion.srcSubresource.baseArrayLayer = 0;
	copiedRegion.srcSubresource.layerCount = 1;
	copiedRegion.srcOffset = vk::Offset3D(srcOffset.x, srcOffset.y, 0);
	copiedRegion.dstSubresource.aspectMask = dstImage->getAspect();
	copiedRegion.dstSubresource.mipLevel = dstMipLevel;
	copiedRegion.dstSubresource.baseArrayLayer = 0;
	copiedRegion.dstSubresource.layerCount = 1;
	copiedRegion.dstOffset = vk::Offset3D(dstOffset.x, dstOffset.y, 0);
	copiedRegion.extent =vk::Extent3D(size.x, size.y, 1);
	
	vk::CopyImageInfo2 copyInfo;
	copyInfo.srcImage = srcImage->getHandle();
	copyInfo.srcImageLayout = srcImage->getLayout();
	copyInfo.dstImage = dstImage->getHandle();
	copyInfo.dstImageLayout = dstImage->getLayout();
	copyInfo.regionCount = 1;
	copyInfo.pRegions = &copiedRegion;
	
	_commandBuffer.copyImage2(copyInfo);
	
	_usedObjects.emplace_back(srcImage);
	_usedObjects.emplace_back(dstImage);
}

void VKCommandBuffer::dispatch(glm::uvec3 groupCount)
{
	if (_boundPipeline == nullptr)
		throw;
	
	if (_boundPipeline->getPipelineType() != vk::PipelineBindPoint::eCompute)
		throw;
	
	_commandBuffer.dispatch(groupCount.x, groupCount.y, groupCount.z);
}

void VKCommandBuffer::pushConstants(vk::ShaderStageFlags shaderStages, const void* data, uint32_t dataSize)
{
	if (_boundPipeline == nullptr)
		throw;
	
	_commandBuffer.pushConstants(_boundPipeline->getPipelineLayout()->getHandle(), shaderStages, 0, dataSize, data);
}

const VKPtr<VKFence>& VKCommandBuffer::getStatusFence()
{
	return _statusFence;
}