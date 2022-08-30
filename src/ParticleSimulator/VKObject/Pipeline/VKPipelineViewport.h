#pragma once

#include <glm/glm.hpp>

struct VKPipelineViewport
{
	glm::ivec2 offset = {};
	glm::ivec2 size = {};
	glm::vec2 depthRange = {};
};