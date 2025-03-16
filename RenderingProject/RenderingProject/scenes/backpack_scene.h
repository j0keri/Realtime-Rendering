#ifndef BACKPACK_SCENE_H
#define BACKPACK_SCENE_H

#include "scene.h"


// A scene with light sources and a lit backpack model
class BackpackScene : public Scene
{
public:
	//-------------------------
	// Vertex and position data
	//-------------------------

	float lightVertices[108] = {
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


	//-------
	// Models
	//-------

	Model backpackModel;


	//--------
	// Shaders
	//--------

	Shader backpackShader;
	Shader lightSourceShader;


	//---------------
	// Buffer objects
	//---------------

	GLuint lightVAO;
	GLuint lightVBO;


	//-----------------
	// Light properties
	//-----------------

	// Directional
	glm::vec3 directionalLightDirection = glm::vec3(-0.2f, -1.0f, -0.3f);
	glm::vec3 directionalLightColor = glm::vec3(0.5f);
	glm::vec3 directionalLightSpecular = glm::vec3(1.0f);

	// Point
	glm::vec3 pointLightPositions[4] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};
	glm::vec3 pointLightColors[4] = {
		glm::vec3(0.5f),
		glm::vec3(0.5f),
		glm::vec3(0.5f),
		glm::vec3(0.5f)
	};
	glm::vec3 pointLightSpeculars[4] = {
		glm::vec3(1.0f),
		glm::vec3(1.0f),
		glm::vec3(1.0f),
		glm::vec3(1.0f)
	};

	// Spot
	glm::vec3 spotLightColor = glm::vec3(1.0f);
	glm::vec3 spotLightSpecular = glm::vec3(1.0f);
	float spotLightInnerCutOff = 12.5f;
	float spotLightOuterCutOff = 17.5f;


	//------
	// Other
	//------

	GLFWwindow *window;  // Window that's rendered into
	Camera *camera;  // Current active camera
	int lightingScheme = 0;
	int amountSchemes = 5;
	bool flashlight = true;
	glm::vec3 skyColor = glm::vec3(0.05f, 0.05f, 0.1f);


	//--------
	// Methods
	//--------

	// Initialize the scene
	BackpackScene(GLFWwindow *w, Camera *c);

	// Render the scene into the window
	void render() override;

	// Handle scene specific keyboard commands
	void handleKey(int key, float deltaTime) override;

	// Adjust light settings, called when lighting scheme is changed
	void adjustLights();
};

#endif