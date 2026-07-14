#include "common.slang"

cbuffer Parameters
{
	float4x4 u_viewProjectionMatrix;
	uint u_particleCount;
	ParticleData* u_inputParticles;
};

struct VIn
{
	uint vertexIndex : SV_VulkanVertexID;
};

struct V2F
{
	float4 rasterPosition: SV_Position;
	float3 color;
};

struct FOut
{
	float4 color : SV_Target0;
};

float3 hsv2rgb(float3 c)
{
	float4 K = float4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
	float3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
	return c.z * lerp(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

float calculateRed(float kelvin)
{
	if (kelvin >= 6600.0)
	{
		float a = 351.97690566805693;
		float b = 0.114206453784165;
		float c = -40.25366309332127;
		float x = (kelvin / 100.0) - 55.0;

		return clamp(a + b * x + c * log(x), 0.0, 255.0);
	}
	else
	{
		return 255.0;
	}
}

float calculateGreen(float kelvin)
{
	if (kelvin < 6600.0)
	{
		float a = -155.25485562709179;
		float b = -0.44596950469579133;
		float c = 104.49216199393888;
		float x = (kelvin / 100.0) - 2.0;

		return clamp(a + b * x + c * log(x), 0.0, 255.0);
	}
	else /*if (kelvin >= 6600.0)*/
	{
		float a = 325.4494125711974;
		float b = 0.07943456536662342;
		float c = -28.0852963507957;
		float x = (kelvin / 100.0) - 50.0;

		return clamp(a + b * x + c * log(x), 0.0, 255.0);
	}
}

float calculateBlue(float kelvin)
{
	float a = -254.76935184120902;
	float b = 0.8274096064007395;
	float c = 115.67994401066147;
	float x = (kelvin / 100.0) - 10.0;

	return clamp(a + b * x + c * log(x), 0.0, 255.0);
}

float3 kelvin2rgb(float kelvin)
{
	return float3(calculateRed(kelvin), calculateGreen(kelvin), calculateBlue(kelvin)) / 255.0;
}

[[shader("vertex")]]
V2F main(VIn input)
{
	ParticleData particle = u_inputParticles[input.vertexIndex];

	float hue = fmod(length(particle.velocity) / 15.0, 1.0);

	V2F output;
	output.rasterPosition = mul(u_viewProjectionMatrix, float4(particle.position, 0.0, 1.0));
	output.color = hsv2rgb(float3(hue, 1.0, 1.0));
	// output.color = kelvin2rgb(length(i_velocity) * 500.0);
	return output;
}

[[shader("fragment")]]
FOut main(V2F input)
{
	FOut output;
	output.color = float4(input.color, 1.0);
	return output;
}