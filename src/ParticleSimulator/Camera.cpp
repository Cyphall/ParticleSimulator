#include "Camera.h"

#include <ParticleSimulator//Helper/MathHelper.h>
#include <ParticleSimulator/RenderContext.h>
#include <ParticleSimulator/VKObject/VKContext.h>

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(glm::vec3 position):
	_position(position)
{
	
}

void Camera::update(float deltaTime)
{
	float ratio = deltaTime * 300;
	
//	if (glfwGetKey(RenderContext::vkContext->getWindow(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
//	{
//		ratio /= 20;
//	}
//	if (glfwGetKey(RenderContext::vkContext->getWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
//	{
//		ratio *= 5;
//	}
//	
//	if (glfwGetKey(RenderContext::vkContext->getWindow(), GLFW_KEY_W) == GLFW_PRESS)
//	{
//		setPosition(getPosition() + glm::vec3(0, 1, 0) * ratio);
//	}
//	if (glfwGetKey(RenderContext::vkContext->getWindow(), GLFW_KEY_S) == GLFW_PRESS)
//	{
//		setPosition(getPosition() - glm::vec3(0, 1, 0) * ratio);
//	}
//	if (glfwGetKey(RenderContext::vkContext->getWindow(), GLFW_KEY_A) == GLFW_PRESS)
//	{
//		setPosition(getPosition() - glm::vec3(1, 0, 0) * ratio);
//	}
//	if (glfwGetKey(RenderContext::vkContext->getWindow(), GLFW_KEY_D) == GLFW_PRESS)
//	{
//		setPosition(getPosition() + glm::vec3(1, 0, 0) * ratio);
//	}
}

glm::mat4 Camera::getView() const
{
	if (_viewChanged) recalculateView();
	return _view;
}

glm::mat4 Camera::getProjection() const
{
	if (_projectionChanged) recalculateProjection();
	return _projection;
}

glm::vec3 Camera::getPosition() const
{
	return _position;
}

void Camera::setPosition(glm::vec3 position)
{
	_position = position;
	viewChanged();
}

glm::ivec2 Camera::getScreenSize() const
{
	return _screenSize;
}

void Camera::setScreenSize(glm::ivec2 screenSize)
{
	_screenSize = screenSize;
	projectionChanged();
}

void Camera::viewChanged() const
{
	_viewChanged = true;
}

void Camera::projectionChanged() const
{
	_projectionChanged = true;
}

void Camera::recalculateView() const
{
	_view = glm::lookAt(getPosition(), getPosition() + glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
	
	_viewChanged = false;
}

void Camera::recalculateProjection() const
{
	glm::vec2 referenceOrthoSize(10, 10);
	glm::vec2 referenceScreenSize(800, 800);
	
	glm::vec2 ratio = static_cast<glm::vec2>(_screenSize) / referenceScreenSize;
	
	glm::vec2 orthoSize = referenceOrthoSize * ratio;
	
	_projection = glm::ortho<float>(0, orthoSize.x, 0, orthoSize.y, -1, 1);
	
	_projectionChanged = false;
}