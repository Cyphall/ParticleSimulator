#pragma once

#include <ParticleSimulator/VKObject/VKObject.h>

#include <vulkan/vulkan.hpp>
#include <filesystem>

template<typename T>
class VKDynamic;
class VKSwapchain;

class VKShader : public VKObject
{
public:
	static VKPtr<VKShader> create(VKContext& context, const std::filesystem::path& path);
	static std::unique_ptr<VKDynamic<VKShader>> createDynamic(VKContext& context, VKSwapchain& swapchain, const std::filesystem::path& path);
	
	~VKShader() override;
	
	vk::ShaderModule getHandle();
	const std::vector<uint32_t>& getCode() const;
	
private:
	VKShader(VKContext& context, const std::filesystem::path& path);
	
	vk::ShaderModule _handle;
	std::vector<uint32_t> _code;
};