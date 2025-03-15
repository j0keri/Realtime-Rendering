#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include "shader.h"
#include "camera.h"
#include "scenes/scene.h"
#include "scenes/box_scene.h"
#include "scenes/light_scene.h"

using namespace std;
using namespace glm;


//--------
// Objects
//--------

Camera camera;
vector<Scene *> scenes;


//----------------
// State variables
//----------------

int currentScene = 0;

bool drawWireframe = false;

bool firstMouse = true;
float lastMouseX = 400.0f;
float lastMouseY = 300.0f;

float deltaTime = 0.0f;
float lastFrameTime = 0.0f;

bool nextSceneKeyAlreadyPressed = false;
bool prevSceneKeyAlreadyPressed = false;
bool wireframeKeyAlreadyPressed = false;


// Viewport resizing when the window is resized
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


// Handle mouse movement
void mouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastMouseX = xPos;
		lastMouseY = yPos;
		firstMouse = false;
	}

	float xOffset = xPos - lastMouseX;
	float yOffset = lastMouseY - yPos; // reversed since y-coordinates range from bottom to top

	lastMouseX = xPos;
	lastMouseY = yPos;

	camera.processMouseMovement(xOffset, yOffset, true);
}


// Handle zooming
void scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	camera.processMouseScroll((float)yOffset / 10.0f);
}


// Keyboard handling
// ESC - exit program
// Right/Left arrow - previous/next scene
// WASD - move camera
// M - toggle rendering mode (solid / wireframe)
// Page up/down - functionality varies per scene
void processInput(GLFWwindow *window)
{
	// ESC
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	// Left arrow
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		if (!prevSceneKeyAlreadyPressed)
		{
			currentScene--;
			if (currentScene < 0)
			{
				currentScene = scenes.size() - 1;
			}
			prevSceneKeyAlreadyPressed = true;
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_RELEASE)
	{
		prevSceneKeyAlreadyPressed = false;
	}

	// Right arrow
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		if (!nextSceneKeyAlreadyPressed)
		{
			currentScene = (currentScene + 1) % scenes.size();
			nextSceneKeyAlreadyPressed = true;
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_RELEASE)
	{
		nextSceneKeyAlreadyPressed = false;
	}

	// W
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.processKeyboard(FORWARD);
	}

	// A
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.processKeyboard(LEFT);
	}

	// S
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.processKeyboard(BACKWARD);
	}

	// D
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.processKeyboard(RIGHT);
	}

	// M
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
	{
		if (!wireframeKeyAlreadyPressed)
		{
			drawWireframe = !drawWireframe;
			wireframeKeyAlreadyPressed = true;
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE)
	{
		wireframeKeyAlreadyPressed = false;
	}

	// Page up
	if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
	{
		scenes[currentScene]->handleKey(GLFW_KEY_PAGE_UP, deltaTime);
	}

	// Page down
	if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
	{
		scenes[currentScene]->handleKey(GLFW_KEY_PAGE_DOWN, deltaTime);
	}
}


//--------------
// Main function
//--------------

int main()
{
	// Initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Create a window
	GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		cerr << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLAD -> load OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cerr << "Failed to initialize GLAD" << endl;
		glfwTerminate();
		return -1;
	}

	// Capture cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Initialize camera and scenes
	camera = Camera(vec3(0.0f, 0.0f, 3.0f));
	scenes.push_back(new BoxScene(window, &camera));
	scenes.push_back(new LightScene(window, &camera));

	// Setup callbacks
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);


	//------------
	// Render loop
	//------------

	while (!glfwWindowShouldClose(window))
	{
		// Calculate deltaTime
		float currentFrameTime = glfwGetTime();
		deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

		// Input handling
		processInput(window);

		// Rendering commands
		glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (drawWireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		// Update camera position
		camera.updatePosition(deltaTime);

		// Render the currently active scene
		scenes[currentScene]->render();

		// Check and call events and swap buffers
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}