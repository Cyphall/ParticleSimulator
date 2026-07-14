#include "common.slang"

cbuffer Parameters
{
	float4x4 u_invViewProjectionMatrix;
	uint u_particleCount;
	ParticleData* u_inputParticles;
	WTexture2D<>::Handle u_outputImage;
	uint2 u_size;
};

float2 remap(float2 value, float2 low1, float2 high1, float2 low2, float2 high2)
{
	return low2 + (value - low1) * (high2 - low2) / (high1 - low1);
}

float2 transformPixelPos(float2 pixelPos, float2 imageSize)
{
	float4 pixelPos4D = float4(remap(pixelPos, float2(0, 0), imageSize, float2(-1, -1), float2(1, 1)), 0, 1);

	pixelPos4D = mul(u_invViewProjectionMatrix, pixelPos4D);
	pixelPos4D /= pixelPos4D.w;

	return float2(pixelPos4D.x, pixelPos4D.y);
}

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

	float force = gravityStrength / pow(distance + 1.0f, power);

	return thisToOther * force;
}

float3 calcPalette(float value)
{
	value = clamp(value, 0.0f, 1.0f);

	float3 a = float3(0.00f, 0.00f, 0.00f);
	float3 b = float3(0.13f, 0.00f, 0.55f);
	float3 c = float3(0.80f, 0.00f, 0.47f);
	float3 d = float3(1.00f, 0.84f, 0.00f);
	float3 e = float3(1.00f, 1.00f, 1.00f);

	if (value <= 0.25f)
	{
		return lerp(a, b, (value - 0.00f) * 4.0f);
	}
	if (value <= 0.50f)
	{
		return lerp(b, c, (value - 0.25f) * 4.0f);
	}
	if (value <= 0.75f)
	{
		return lerp(c, d, (value - 0.50f) * 4.0f);
	}
	if (value <= 1.00f)
	{
		return lerp(d, e, (value - 0.75f) * 4.0f);
	}

	return float3(0.0f, 1.0f, 0.0f);
}

[[shader("compute")]]
[[numthreads(32, 32, 1)]]
void main(uint3 threadID : SV_DispatchThreadID)
{
	uint2 pixelPos = threadID.xy;
	if (any(pixelPos >= u_size))
	{
		return;
	}

	float2 pixelWorldPos = transformPixelPos(float2(pixelPos) + 0.5f, u_size);

	float gravity = 0.0f;

	for (uint i = 0; i < u_particleCount; i++)
	{
		float2 particlePos = u_inputParticles[i].position;

		gravity += length(calcGravity(pixelWorldPos, particlePos, 0.1f, 8.0f));
	}

	u_outputImage.Store(pixelPos, float4(calcPalette(gravity), 1));
}