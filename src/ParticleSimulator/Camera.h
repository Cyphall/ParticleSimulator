#pragma once

#include <glm/glm.hpp>

class Camera
{
public:
	explicit Camera(glm::vec3 position = glm::vec3(0));
	
	void update(float deltaTime);
	
	glm::mat4 getView() const;
	
	glm::mat4 getProjection() const;
	
	glm::vec3 getPosition() const;
	void setPosition(glm::vec3 position);
	
	glm::ivec2 getScreenSize() const;
	void setScreenSize(glm::ivec2 screenSize);

private:
	void viewChanged() const;
	mutable bool _viewChanged = true;
	mutable glm::mat4 _view = glm::mat4(0);
	
	void projectionChanged() const;
	mutable bool _projectionChanged = true;
	mutable glm::mat4 _projection = glm::mat4(0);
	
	glm::vec3 _position;
	
	double _previousTime = 0.0;
	
	glm::ivec2 _screenSize = {0, 0};
	
	void recalculateView() const;
	void recalculateProjection() const;
};