#include "camera.h"

using namespace std;
using namespace glm;


Camera::Camera(vec3 position, vec3 up, float yaw, float pitch) :
	front(vec3(0.0f, 0.0f, -1.0f)),
	movementSpeed(SPEED),
	mouseSensitivity(SENSITIVITY),
	fov(FOV)
{
	this->position = position;
	this->worldUp = up;
	this->yaw = yaw;
	this->pitch = pitch;
	updateCameraVectors();
}


Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) :
	front(vec3(0.0f, 0.0f, -1.0f)),
	movementSpeed(SPEED),
	mouseSensitivity(SENSITIVITY),
	fov(FOV)
{
	this->position = vec3(posX, posY, posZ);
	this->worldUp = vec3(upX, upY, upZ);
	this->yaw = yaw;
	this->pitch = pitch;
	updateCameraVectors();
}


mat4 Camera::getViewMatrix() const
{
	return lookAt(position, position + front, up);
}


void Camera::updatePosition(float deltaTime)
{
	vec2 movement(0.0f);
	float velocity = movementSpeed * deltaTime;
	
	if (moveForward)
	{
		movement += vec2(1.0f, 0.0f);
	}
	if (moveBack)
	{
		movement -= vec2(1.0f, 0.0f);
	}
	if (moveLeft)
	{
		movement -= vec2(0.0f, 1.0f);
	}
	if (moveRight)
	{
		movement += vec2(0.0f, 1.0f);
	}
	
	if (length(movement) != 0.0f)
	{
		movement = normalize(movement);
	}
	
	position += front * movement.x * velocity;
	position += right * movement.y * velocity;
	
	moveForward = moveBack = moveLeft = moveRight = false;
}


void Camera::processKeyboard(CameraMovement direction)
{
	if (direction == FORWARD)
	{
		moveForward = true;
	}
	if (direction == BACKWARD)
	{
		moveBack = true;
	}
	if (direction == LEFT)
	{
		moveLeft = true;
	}
	if (direction == RIGHT)
	{
		moveRight = true;
	}
}


void Camera::processMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch)
{
	xOffset *= mouseSensitivity;
	yOffset *= mouseSensitivity;

	yaw += xOffset;
	pitch += yOffset;

	// Make sure that when pitch is out of bounds screen doesn't get flipped
	if (constrainPitch)
	{
		if (pitch > 89.0f)
		{
			pitch = 89.0f;
		}
		if (pitch < -89.0f)
		{
			pitch = -89.0f;
		}
	}

	// Update front, right and up vectors using the updated euler angles
	updateCameraVectors();
}


void Camera::processMouseScroll(float yOffset)
{
	fov -= (float)yOffset;
	if (fov < 1.0f)
	{
		fov = 1.0f;
	}
	if (fov > 45.0f)
	{
		fov = 45.0f;
	}
}


void Camera::updateCameraVectors()
{
	// Calculate the new front vector
	vec3 newFront;
	newFront.x = cos(radians(yaw)) * cos(radians(pitch));
	newFront.y = sin(radians(pitch));
	newFront.z = sin(radians(yaw)) * cos(radians(pitch));
	front = normalize(newFront);
	// Also re-calculate the right and up vector
	right = normalize(cross(front, worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement
	up = normalize(cross(right, front));
}

