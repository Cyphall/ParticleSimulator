#include "RenderContext.h"

#include <ParticleSimulator/VKObject/VKContext.h>
#include <ParticleSimulator/VKObject/VKSwapchain.h>
#include <ParticleSimulator/Rendering/Renderer.h>

std::unique_ptr<VKContext> RenderContext::vkContext;
std::unique_ptr<VKSwapchain> RenderContext::swapchain;
std::unique_ptr<Renderer> RenderContext::renderer;