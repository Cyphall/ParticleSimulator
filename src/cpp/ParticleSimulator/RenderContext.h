#pragma once

#include <memory>

class VKContext;
class VKSwapchain;
class Renderer;

struct RenderContext
{
	static std::unique_ptr<VKContext> vkContext;
	static std::unique_ptr<VKSwapchain> swapchain;
	static std::unique_ptr<Renderer> renderer;
};