#include "FileHelper.h"

#include <fstream>

std::vector<uint32_t> FileHelper::readSPIRV(const std::filesystem::path& filePath)
{
	uint64_t size = std::filesystem::file_size(filePath);
	if (size % sizeof(uint32_t) != 0)
	{
		throw;
	}
	
	std::ifstream file(filePath, std::ios::in | std::ios::binary);
	
	std::vector<uint32_t> data(size / sizeof(uint32_t));
	file.read(reinterpret_cast<char*>(data.data()), size);
	
	return data;
}