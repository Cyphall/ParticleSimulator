#include "VKShader.h"

#include <ParticleSimulator/VKObject/VKContext.h>
#include <ParticleSimulator/VKObject/VKDynamic.h>

#include <vector>
#include <cmrc/cmrc.hpp>

CMRC_DECLARE(shaders);

VKPtr<VKShader> VKShader::create(VKContext& context, const std::filesystem::path& path)
{
	return VKPtr<VKShader>(new VKShader(context, path));
}

std::unique_ptr<VKDynamic<VKShader>> VKShader::createDynamic(VKContext& context, VKSwapchain& swapchain, const std::filesystem::path& path)
{
	return std::unique_ptr<VKDynamic<VKShader>>(new VKDynamic<VKShader>(context, swapchain, path));
}

VKShader::VKShader(VKContext& context, const std::filesystem::path& path): VKObject(context)
{
	cmrc::file spirvFile = cmrc::shaders::get_filesystem().open(std::format("{}.spv", path.generic_string()));

	if ((spirvFile.end() - spirvFile.begin()) % sizeof(uint32_t) != 0)
	{
		throw;
	}

	_code = {
		reinterpret_cast<const uint32_t*>(spirvFile.begin()),
		reinterpret_cast<const uint32_t*>(spirvFile.end())
	};

	vk::ShaderModuleCreateInfo createInfo;
	createInfo.codeSize = _code.size() * sizeof(uint32_t);
	createInfo.pCode = _code.data();

	_handle = _context.getDevice().createShaderModule(createInfo);
}

VKShader::~VKShader()
{
	_context.getDevice().destroyShaderModule(_handle);
}

vk::ShaderModule VKShader::getHandle()
{
	return _handle;
}

const std::vector<uint32_t>& VKShader::getCode() const
{
	return _code;
}