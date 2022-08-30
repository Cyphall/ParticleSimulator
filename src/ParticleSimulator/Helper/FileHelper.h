#pragma once

#include <vector>
#include <filesystem>

class FileHelper
{
public:
	static std::vector<uint32_t> readSPIRV(const std::filesystem::path& filePath);
};