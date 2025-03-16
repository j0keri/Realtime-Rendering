#ifndef SCENE_H
#define SCENE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../shader.h"
#include "../camera.h"
#include "../texture_legacy.h"
#include "../model.h"


// Base class for scenes
// NOTE: I made the design choice that every scene should be self contained,
//       and as such they shall not share resources to keep things clear, meaning
//       data may and will be duplicated, but that's a sacrifice I'm willing to make here
class Scene
{
public:
	// Render the scene into the window
	virtual void render() = 0;

	// Handle scene specific keyboard commands
	virtual void handleKey(int key, float deltaTime) = 0;
};

#endif