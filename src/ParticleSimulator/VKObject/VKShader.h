#pragma once

#include <vulkan/vulkan.hpp>
#include <filesystem>

class VKShader
{
public:
	explicit VKShader(const std::filesystem::path& path);
	~VKShader();
	
	vk::ShaderModule getHandle();
	const std::vector<uint32_t>& getCode() const;
	
private:
	vk::ShaderModule _handle;
	std::vector<uint32_t> _code;
};
