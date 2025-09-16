#pragma once

#include <glm/glm.hpp>

struct VKPipelineViewport
{
	glm::uvec2 offset = {};
	glm::uvec2 size = {};
	glm::vec2 depthRange = {};
};