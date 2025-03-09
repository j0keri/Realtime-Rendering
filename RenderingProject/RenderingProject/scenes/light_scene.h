#ifndef LIGHT_SCENE_H
#define LIGHT_SCENE_H

#include "scene.h"


// A scene with a light source and lit boxes
class LightScene : public Scene
{
public:
	//------------
	// Vertex data
	//------------

	float boxVertices[108] = {
		// Positions        
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f
	};


	//---------
	// Textures
	//---------

	// None


	//--------
	// Shaders
	//--------

	Shader boxShader;
	Shader lightSourceShader;


	//---------------
	// Buffer objects
	//---------------

	GLuint lightVAO;
	GLuint boxVAO;
	GLuint boxVBO;


	//------
	// Other
	//------

	GLFWwindow *window;  // Window that's rendered into
	Camera *camera;  // Current active camera

	glm::vec3 coralColor = glm::vec3(1.0f, 0.5f, 0.31f);
	glm::vec3 whiteColor = glm::vec3(1.0f, 1.0f, 1.0f);

	glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);


	//--------
	// Methods
	//--------

	// Initialize the scene
	LightScene(GLFWwindow *w, Camera *c);

	// Render the scene into the window
	void render() override;

	// Handle scene specific keyboard commands
	void handleKey(int key) override;
};

#endif