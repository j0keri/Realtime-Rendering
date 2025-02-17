#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <shader.h>

using namespace std;


//------------
// Vertex data
//------------

// Vertices and indices for example triangles
float vertices[] = {
	 0.5f,  0.5f, 0.0f,  // Top right
	 0.5f, -0.5f, 0.0f,  // Bottom right
	-0.5f, -0.5f, 0.0f,  // Bottom left
	-0.5f,  0.5f, 0.0f   // Top left 
};
unsigned int indices[] = {
	0, 1, 3,   // First triangle
	1, 2, 3    // Second triangle
};

// Vertices for exercise triangle 1
float verticesEx1[] = {
	-0.5f,  0.0f, 0.0f,
	 0.0f,  0.5f, 0.0f,
	 0.0f, -0.5f, 0.0f
};

// Vertices for exercise triangle 2
float verticesEx2[] = {
	 0.5f,  0.0f, 0.0f,
	 0.0f, -0.5f, 0.0f,
	 0.0f,  0.5f, 0.0f
};


//------------------------
// Shader source filepaths
//------------------------

// Vertex
const char *vertBasicPath = "shaders/vert_basic.vs"; // Basic vertex shader, no processing on verts

// Fragment
const char *fragBasicPath = "shaders/frag_basic.fs"; // Basic fragment shader (orange)
const char *fragYellowPath = "shaders/frag_yellow.fs"; // Yellow fragment shader


//----------------
// State variables
//----------------

int scene = 0;
bool drawWireframe = false;
bool sceneKeyAlreadyPressed = false;
bool wireframeKeyAlreadyPressed = false;


// Viewport resizing when the window is resized
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


// Keyboard handling
// ESC - exit program
// W - toggle wireframe mode
// S - toggle between drawing the example triangles and the exercise triangles
void processInput(GLFWwindow *window)
{
	// ESC
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	// W
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		if (!wireframeKeyAlreadyPressed) {
			drawWireframe = !drawWireframe;
			wireframeKeyAlreadyPressed = true;
		}
	}
	else if(glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE)
	{
		wireframeKeyAlreadyPressed = false;
	}

	// S
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		if (!sceneKeyAlreadyPressed) {
			scene = (scene + 1) % 2;
			sceneKeyAlreadyPressed = true;
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE)
	{
		sceneKeyAlreadyPressed = false;
	}
}


int main()
{
	// Initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Create a window
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
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

	// Setup callbacks
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	// Shaders (using own Shader class to compile, link, and access programs to reduce the amount of copy paste)
	Shader shaderBasic(vertBasicPath, fragBasicPath);
	Shader shaderBasicYellow(vertBasicPath, fragYellowPath);

	//--------------------

	// Generate vertex array objects, vertex buffer objects and element buffer object
	unsigned int VAO[3];
	unsigned int VBO[3];
	unsigned int EBO;
	glGenVertexArrays(3, VAO);
	glGenBuffers(3, VBO);
	glGenBuffers(1, &EBO);

	//---------------------------------------
	// VAO[0] (the one described in material)
	//---------------------------------------

	// Bind vertex array object
	glBindVertexArray(VAO[0]);

	// Copy vertex data into buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Copy index data into buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Setup vertex attribute pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Unbind vertex array object
	glBindVertexArray(0);

	//---------------------------------------
	// VAO[1] (for exercises, first triangle)
	//---------------------------------------

	// Bind vertex array object
	glBindVertexArray(VAO[1]);

	// Copy vertex data into buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesEx1), verticesEx1, GL_STATIC_DRAW);

	// Setup vertex attribute pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Unbind vertex array object
	glBindVertexArray(0);

	//----------------------------------------
	// VAO[2] (for exercises, second triangle)
	//----------------------------------------

	// Bind vertex array object
	glBindVertexArray(VAO[2]);

	// Copy vertex data into buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesEx2), verticesEx2, GL_STATIC_DRAW);

	// Setup vertex attribute pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Unbind vertex array object
	glBindVertexArray(0);

	//--------------------

	// Render loop
	while (!glfwWindowShouldClose(window))
	{
		// Input
		processInput(window);

		// Rendering commands
		glClearColor(0.5f, 0.1f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		if (drawWireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		switch (scene)
		{
			case 0:
				shaderBasic.use();
				glBindVertexArray(VAO[0]);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
				break;
			case 1:
				shaderBasic.use();
				glBindVertexArray(VAO[1]);
				glDrawArrays(GL_TRIANGLES, 0, 3);

				shaderBasicYellow.use();
				glBindVertexArray(VAO[2]);
				glDrawArrays(GL_TRIANGLES, 0, 3);

				glBindVertexArray(0);
				break;
		}

		// Check and call events and swap buffers
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}