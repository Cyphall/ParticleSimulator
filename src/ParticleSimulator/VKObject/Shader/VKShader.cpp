#include "VKShader.h"

#include <ParticleSimulator/VKObject/VKContext.h>
#include <ParticleSimulator/VKObject/VKDynamic.h>
#include <ParticleSimulator/Helper/FileHelper.h>

#include <vector>

VKPtr<VKShader> VKShader::create(VKContext& context, const std::filesystem::path& path)
{
	return VKPtr<VKShader>(new VKShader(context, path));
}

std::unique_ptr<VKDynamic<VKShader>> VKShader::createDynamic(VKContext& context, VKSwapchain& swapchain, const std::filesystem::path& path)
{
	return std::unique_ptr<VKDynamic<VKShader>>(new VKDynamic<VKShader>(context, swapchain, path));
}

VKShader::VKShader(VKContext& context, const std::filesystem::path& path):
	VKObject(context)
{
	_code = FileHelper::readSPIRV(path);
	
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