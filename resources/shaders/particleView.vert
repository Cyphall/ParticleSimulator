#version 460

layout(location = 0) in vec2 i_position;
layout(location = 1) in vec2 i_velocity;

layout(push_constant) uniform constants
{
	mat4 u_vp;
};

layout(location = 0) out vec3 o_color;

vec3 hsv2rgb(vec3 c)
{
	vec4 K = vec4(1.0f, 2.0f / 3.0f, 1.0f / 3.0f, 3.0f);
	vec3 p = abs(fract(c.xxx + K.xyz) * 6.0f - K.www);
	return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0f, 1.0f), c.y);
}

float calculateRed(float kelvin)
{
	if (kelvin >= 6600.0f)
	{
		float a = 351.97690566805693f;
		float b = 0.114206453784165f;
		float c = -40.25366309332127f;
		float x = (kelvin / 100.0f) - 55.0f;
		
		return clamp(a + b * x + c * log(x), 0.0f, 255.0f);
	}
	else
	{
		return 255.0f;
	}
}

float calculateGreen(float kelvin)
{
	if (kelvin < 6600.0f)
	{
		float a = -155.25485562709179f;
		float b = -0.44596950469579133f;
		float c = 104.49216199393888f;
		float x = (kelvin / 100.0f) - 2.0f;
		
		return clamp(a + b * x + c * log(x), 0.0f, 255.0f);
	}
	else /*if (kelvin >= 6600.0f)*/
	{
		float a = 325.4494125711974f;
		float b = 0.07943456536662342f;
		float c = -28.0852963507957f;
		float x = (kelvin / 100.0f) - 50.0f;
		
		return clamp(a + b * x + c * log(x), 0.0f, 255.0f);
	}
}

float calculateBlue(float kelvin)
{
	float a = -254.76935184120902f;
	float b = 0.8274096064007395f;
	float c = 115.67994401066147f;
	float x = (kelvin / 100.0f) - 10.0f;
	
	return clamp(a + b * x + c * log(x), 0.0f, 255.0f);
}

vec3 kelvin2rgb(float kelvin)
{
	return vec3(calculateRed(kelvin), calculateGreen(kelvin), calculateBlue(kelvin)) / 255.0f;
}

void main()
{
	gl_PointSize = 1.0f;
	gl_Position = u_vp * vec4(i_position, 0.0f, 1.0f);
	
	float hue = mod(length(i_velocity) / 15.0f, 1.0f);
	o_color = hsv2rgb(vec3(hue, 1.0f, 1.0f));
	
//	o_color = kelvin2rgb(length(i_velocity) * 500.0f);
}