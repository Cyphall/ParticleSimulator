#include "VKShader.h"

#include <ParticleSimulator/RenderContext.h>
#include <ParticleSimulator/Helper/FileHelper.h>
#include <ParticleSimulator/VKObject/VKContext.h>

#include <vector>

VKShader::VKShader(const std::filesystem::path& path)
{
	_code = FileHelper::readSPIRV(path);
	
	vk::ShaderModuleCreateInfo createInfo;
	createInfo.codeSize = _code.size() * sizeof(uint32_t);
	createInfo.pCode = _code.data();
	
	_handle = RenderContext::vkContext->getDevice().createShaderModule(createInfo);
}

VKShader::~VKShader()
{
	RenderContext::vkContext->getDevice().destroyShaderModule(_handle);
}

vk::ShaderModule VKShader::getHandle()
{
	return _handle;
}

const std::vector<uint32_t>& VKShader::getCode() const
{
	return _code;
}