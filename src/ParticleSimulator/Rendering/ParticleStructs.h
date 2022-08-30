#pragma once

#include <glm/glm.hpp>

struct ParticleData
{
	alignas(8) glm::vec2 position;
	alignas(8) glm::vec2 velocity;
};