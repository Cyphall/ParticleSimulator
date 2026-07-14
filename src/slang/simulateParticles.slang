#include "common.slang"

cbuffer Parameters
{
	float u_deltaTime;
	uint u_particleCount;
	float2 u_cursorGravityPos;
	bool u_cursorGravityEnabled;
	bool u_particlesGravityEnabled;
	ParticleData* u_inputParticles;
	ParticleData* u_outputParticles;
};

float2 calcGravity(float2 thisPosition, float2 otherPosition, float gravityStrength, float power)
{
	float2 thisToOther = otherPosition - thisPosition;
	float distance = length(thisToOther);
	if (distance > 0)
	{
		thisToOther /= distance;
	}
	else
	{
		thisToOther = float2(0);
	}

	float force = gravityStrength / pow(distance + 1.0, power);

	return thisToOther * force * u_deltaTime;
}

float calcFalloff(float falloffPerSecond)
{
	float falloff = falloffPerSecond;
	falloff = 1.0 - falloff;
	falloff *= u_deltaTime;
	return 1.0 - falloff;
}

[[shader("compute")]]
[[numthreads(1024, 1, 1)]]
void main(uint3 threadID : SV_DispatchThreadID)
{
	uint particleIndex = threadID.x;
	if (particleIndex >= u_particleCount)
	{
		return;
	}

	ParticleData particle = u_inputParticles[particleIndex];

	if (u_cursorGravityEnabled)
	{
		particle.velocity += calcGravity(particle.position, u_cursorGravityPos, 50.0, 2.0);
	}

	if (u_particlesGravityEnabled)
	{
		for (uint i = 0; i < u_particleCount; i++)
		{
			if (i == particleIndex)
			{
				continue;
			}

			particle.velocity += calcGravity(particle.position, u_inputParticles[i].position, 0.01, 4.0);
		}
	}

	particle.velocity *= calcFalloff(0.9);

	particle.position += particle.velocity * u_deltaTime;

	u_outputParticles[particleIndex] = particle;

	// semi-accurate bounce algorithm
//	float2 potentialDeltaPosition = particle.velocity * u_deltaTime;
//	float2 potentialNextPosition = particle.position + potentialDeltaPosition;
//
//	float2 velocityRatioBeforeSwap = float2(1.0, 1.0);
//
//	if (potentialNextPosition.x < 0.0)
//	{
//		float relativeWallPos = 0.0 - particle.position.x;
//		velocityRatioBeforeSwap.x = relativeWallPos / potentialDeltaPosition.x;
//	}
//	else if (potentialNextPosition.x > 10.0)
//	{
//		float relativeWallPos = 10.0 - particle.position.x;
//		velocityRatioBeforeSwap.x = relativeWallPos / potentialDeltaPosition.x;
//	}
//
//	if (potentialNextPosition.y < 0.0)
//	{
//		float relativeWallPos = 0.0 - particle.position.y;
//		velocityRatioBeforeSwap.y = relativeWallPos / potentialDeltaPosition.y;
//	}
//	else if (potentialNextPosition.y > 10.0)
//	{
//		float relativeWallPos = 10.0 - particle.position.y;
//		velocityRatioBeforeSwap.y = relativeWallPos / potentialDeltaPosition.y;
//	}
//
//	particle.position += particle.velocity * u_deltaTime * velocityRatioBeforeSwap;
//	if (velocityRatioBeforeSwap.x < 1.0)
//	{
//		particle.velocity.x = -particle.velocity.x;
//		particle.position.x += particle.velocity.x * u_deltaTime * (1.0 - velocityRatioBeforeSwap.x);
//	}
//	if (velocityRatioBeforeSwap.y < 1.0)
//	{
//		particle.velocity.y = -particle.velocity.y;
//		particle.position.y += particle.velocity.y * u_deltaTime * (1.0 - velocityRatioBeforeSwap.y);
//	}
}