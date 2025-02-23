#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <shader.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;


//------------
// Vertex data
//------------

// Vertices and indices for example triangles
float vertices[] = {
	 // Positions        // Colors          // Texture coords
	 0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 0.0f,  1.0f, 1.0f,  // Top right
	 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,  // Bottom right
	-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,  // Bottom left
	-0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f   // Top left 
};
unsigned int indices[] = {
	0, 1, 3,  // First triangle
	1, 2, 3   // Second triangle
};

// Vertices for "Hello Triangle" exercise triangle 1
float verticesEx1[] = {
	-0.5f,  0.0f, 0.0f,
	 0.0f,  0.5f, 0.0f,
	 0.0f, -0.5f, 0.0f
};

// Vertices for "Hello Triangle" exercise triangle 2
float verticesEx2[] = {
	 0.5f,  0.0f, 0.0f,
	 0.0f, -0.5f, 0.0f,
	 0.0f,  0.5f, 0.0f
};


//------------------------
// Shader source filepaths
//------------------------

// Vertex
const char *vertBasicPath = "shaders/vert_basic.vs";  // Basic vertex shader, no processing on verts
const char *vertColorPath = "shaders/vert_color.vs";  // Vertex shader that sends color data to fragment
const char *vertColorFromPosPath = "shaders/vert_colorFromPos.vs";  // Vertex shader that sends color data to fragment, the data is the vertex position
const char *vertTexturePath = "shaders/vert_texture.vs";  // Vertex shader that uses texture coordinates

// Fragment
const char *fragBasicPath = "shaders/frag_basic.fs";  // Basic fragment shader (orange)
const char *fragYellowPath = "shaders/frag_yellow.fs";  // Yellow fragment shader
const char *fragVertexColorPath = "shaders/frag_vertexColor.fs";  // Fragment shader with color set by vertex shader
const char *fragUniformPath = "shaders/frag_uniform.fs";  // Fragment shader with color set via uniform
const char *fragTexturePath = "shaders/frag_texture.fs";  // Fragment shader that samples a texture
const char *fragTextureVertexColorMixPath = "shaders/frag_textureVertexColorMix.fs";  // Fragment shader that samples a texture and mixes it with color set by vertex shader
const char *fragTexture2xMixPath = "shaders/frag_texture2xMix.fs";  // Fragment shader that samples two textures and mixes them according to a weight given via uniform


//----------------
// State variables
//----------------

int scene = 0;
int sceneAmount = 7;

bool drawWireframe = false;
bool flipped = false;

float offsetX = 0.0f;
float offsetY = 0.0f;

float textureMix = 0.2;

bool sceneKeyAlreadyPressed = false;
bool wireframeKeyAlreadyPressed = false;
bool flipKeyAlreadyPressed = false;
bool upKeyAlreadyPressed = false;
bool leftKeyAlreadyPressed = false;
bool downKeyAlreadyPressed = false;
bool rightKeyAlreadyPressed = false;
bool pgUpKeyAlreadyPressed = false;
bool pgDownKeyAlreadyPressed = false;


// Viewport resizing when the window is resized
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


// Keyboard handling
// ESC - exit program
// W - toggle wireframe mode
// S - toggle between scenes
// F - vertically flip triangles that can be flipped
// Arrows - adjust offset for triangles that use offset values
// Page up/down - adjust texture blending
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
			scene = (scene + 1) % sceneAmount;
			sceneKeyAlreadyPressed = true;
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE)
	{
		sceneKeyAlreadyPressed = false;
	}
	// F
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
	{
		if (!flipKeyAlreadyPressed) {
			flipped = !flipped;
			flipKeyAlreadyPressed = true;
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE)
	{
		flipKeyAlreadyPressed = false;
	}
	// Up
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	{
		if (!upKeyAlreadyPressed) {
			offsetY += 0.2f;
			upKeyAlreadyPressed = true;
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE)
	{
		upKeyAlreadyPressed = false;
	}
	// Left
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	{
		if (!leftKeyAlreadyPressed) {
			offsetX -= 0.2f;
			leftKeyAlreadyPressed = true;
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_RELEASE)
	{
		leftKeyAlreadyPressed = false;
	}
	// Down
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	{
		if (!downKeyAlreadyPressed) {
			offsetY -= 0.2f;
			downKeyAlreadyPressed = true;
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE)
	{
		downKeyAlreadyPressed = false;
	}
	// Right
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	{
		if (!rightKeyAlreadyPressed) {
			offsetX += 0.2f;
			rightKeyAlreadyPressed = true;
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_RELEASE)
	{
		rightKeyAlreadyPressed = false;
	}
	// Page up
	if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
	{
		if (!pgUpKeyAlreadyPressed) {
			textureMix += 0.2f;
			pgUpKeyAlreadyPressed = true;

			if (textureMix > 1.0f)
			{
				textureMix = 1.0f;
			}
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_RELEASE)
	{
		pgUpKeyAlreadyPressed = false;
	}
	// Page down
	if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
	{
		if (!pgDownKeyAlreadyPressed) {
			textureMix -= 0.2f;
			pgDownKeyAlreadyPressed = true;

			if (textureMix < 0.0f)
			{
				textureMix = 0.0f;
			}
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_RELEASE)
	{
		pgDownKeyAlreadyPressed = false;
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


	//---------
	// Textures
	//---------

	// Flip texture images vertically
	stbi_set_flip_vertically_on_load(true);

	// Container:
	// Generate texture
	GLuint textureContainer;
	glGenTextures(1, &textureContainer);
	glBindTexture(GL_TEXTURE_2D, textureContainer);

	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Load texture image
	int width, height, nrChannels;
	unsigned char *data = stbi_load("textures/container.jpg", &width, &height, &nrChannels, 0);

	// Assign image to texture and generate mipmaps
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		cerr << "Failed to load texture" << endl;
	}

	// Awesomeface:
	// Generate texture
	GLuint textureAwesomeface;
	glGenTextures(1, &textureAwesomeface);
	glBindTexture(GL_TEXTURE_2D, textureAwesomeface);

	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Load texture image
	data = stbi_load("textures/awesomeface.png", &width, &height, &nrChannels, 0);

	// Assign image to texture and generate mipmaps
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		cerr << "Failed to load texture" << endl;
	}

	// Free the image memory
	stbi_image_free(data);


	//----------------------------------------------------------------------------------------------------------
	// Shaders (using own Shader class to compile, link, and access programs to reduce the amount of copy paste)
	//----------------------------------------------------------------------------------------------------------

	Shader shaderBasic(vertBasicPath, fragBasicPath);
	Shader shaderBasicYellow(vertBasicPath, fragYellowPath);
	Shader shaderVertexColor(vertColorPath, fragVertexColorPath);
	Shader shaderBasicUniformColor(vertBasicPath, fragUniformPath);
	Shader shaderVertexColorFromPos(vertColorFromPosPath, fragVertexColorPath);
	Shader shaderTexture(vertTexturePath, fragTexturePath);
	Shader shaderTextureVertexColorMix(vertTexturePath, fragTextureVertexColorMixPath);
	Shader shaderTexture2xMix(vertTexturePath, fragTexture2xMixPath);

	// Set samplers
	shaderTexture.use();
	shaderTexture.setInt("ourTexture", 0);

	shaderTextureVertexColorMix.use();
	shaderTextureVertexColorMix.setInt("ourTexture", 0);

	shaderTexture2xMix.use();
	shaderTexture2xMix.setInt("tex0", 0);
	shaderTexture2xMix.setInt("tex1", 1);


	//--------------------

	// Generate vertex array objects, vertex buffer objects and element buffer object
	unsigned int VAO[3];
	unsigned int VBO[3];
	unsigned int EBO;
	glGenVertexArrays(3, VAO);
	glGenBuffers(3, VBO);
	glGenBuffers(1, &EBO);

	// TODO: Refactor the following to be more readable (VAO-class maybe?)

	//-----------------------------------------------------------------------------
	// VAO[0] (the one described in material) (modified to have color data as well)
	//-----------------------------------------------------------------------------

	// Bind vertex array object
	glBindVertexArray(VAO[0]);

	// Copy vertex data into buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Copy index data into buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Setup vertex position attribute pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Setup vertex color attribute pointers
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Setup vertex texture coordinate attribute pointers
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

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
			// "Hello Triangle" example
			case 0:
				// Draw two orange triangles
				shaderBasic.use();
				shaderBasic.setBool("flip", flipped);
				shaderBasic.setFloat("offsetX", offsetX);
				shaderBasic.setFloat("offsetY", offsetY);
				glBindVertexArray(VAO[0]);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
				break;
			// "Hello Triangle" exercises
			case 1:
				// Draw orange triangle
				shaderBasic.use();
				shaderBasic.setBool("flip", flipped);
				shaderBasic.setFloat("offsetX", offsetX);
				shaderBasic.setFloat("offsetY", offsetY);
				glBindVertexArray(VAO[1]);
				glDrawArrays(GL_TRIANGLES, 0, 3);

				// Draw yellow triangle
				shaderBasicYellow.use();
				shaderBasicYellow.setBool("flip", flipped);
				shaderBasicYellow.setFloat("offsetX", offsetX);
				shaderBasicYellow.setFloat("offsetY", offsetY);
				glBindVertexArray(VAO[2]);
				glDrawArrays(GL_TRIANGLES, 0, 3);
				break;
			// "Shaders" example
			case 2:
				// Draw black to green fading triangle
				shaderBasicUniformColor.use();
				shaderBasicUniformColor.setBool("flip", flipped);
				shaderBasicUniformColor.setFloat("offsetX", offsetX);
				shaderBasicUniformColor.setFloat("offsetY", offsetY);
				{
					float time = glfwGetTime();
					float green = (sin(time * 4) / 2.0f) + 0.5f;
					shaderBasicUniformColor.setVec4f("ourColor", 0.0f, green, 0.0f, 1.0f);
				}
				glBindVertexArray(VAO[0]);
				glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

				// Draw RGB triangle
				shaderVertexColor.use();
				shaderVertexColor.setBool("flip", flipped);
				shaderVertexColor.setFloat("offsetX", offsetX);
				shaderVertexColor.setFloat("offsetY", offsetY);
				glBindVertexArray(VAO[0]);
				glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (const void*)(3 * sizeof(GLuint)));
				break;
			// "Shaders" exercise 3 (1 and 2 also work for prior scenes)
			case 3:
				// Draw two triangles that get their color value from vertex position
				shaderVertexColorFromPos.use();
				shaderVertexColorFromPos.setBool("flip", flipped);
				shaderVertexColorFromPos.setFloat("offsetX", offsetX);
				shaderVertexColorFromPos.setFloat("offsetY", offsetY);
				glBindVertexArray(VAO[0]);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
				break;
			// "Textures" example 1
			case 4:
				// Draw triangle that has the "container" texture
				shaderTexture.use();
				shaderTexture.setBool("flip", flipped);
				shaderTexture.setFloat("offsetX", offsetX);
				shaderTexture.setFloat("offsetY", offsetY);
				{
					mat4 transform(1.0f);
					shaderTexture.setMat4f("transform", transform);
				}
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, textureContainer);
				glBindVertexArray(VAO[0]);
				glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

				// Draw triangle that has the "container" texture mixed with vertex color
				shaderTextureVertexColorMix.use();
				shaderTextureVertexColorMix.setBool("flip", flipped);
				shaderTextureVertexColorMix.setFloat("offsetX", offsetX);
				shaderTextureVertexColorMix.setFloat("offsetY", offsetY);
				{
					mat4 transform(1.0f);
					shaderTexture.setMat4f("transform", transform);
				}
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, textureContainer);
				glBindVertexArray(VAO[0]);
				glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (const void*)(3 * sizeof(GLuint)));
				break;
			// "Textures" example 2
			case 5:
				// Draw two triangles with the "container" texture and the "awesomeface" texture mixed
				shaderTexture2xMix.use();
				shaderTexture2xMix.setBool("flip", flipped);
				shaderTexture2xMix.setFloat("offsetX", offsetX);
				shaderTexture2xMix.setFloat("offsetY", offsetY);
				shaderTexture2xMix.setFloat("mixWeight", textureMix);
				{
					mat4 transform(1.0f);
					shaderTexture2xMix.setMat4f("transform", transform);
				}
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, textureContainer);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, textureAwesomeface);
				glBindVertexArray(VAO[0]);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
				break;
			// "Transformations" example and exercises
			case 6:
				// Draw two triangles with the "container" texture and the "awesomeface" texture mixed
				// Rotate them counter-clockwise over time, scale them by 0.5, and move them to the bottom right corner
				shaderTexture2xMix.use();
				shaderTexture2xMix.setBool("flip", flipped);
				shaderTexture2xMix.setFloat("offsetX", offsetX);
				shaderTexture2xMix.setFloat("offsetY", offsetY);
				shaderTexture2xMix.setFloat("mixWeight", textureMix);
				{
					mat4 transform(1.0f);
					transform = translate(transform, vec3(0.5f, -0.5f, 0.0f));
					transform = rotate(transform, (float)glfwGetTime(), vec3(0.0f, 0.0f, 1.0f));
					transform = scale(transform, vec3(0.5f, 0.5f, 0.5f));
					shaderTexture2xMix.setMat4f("transform", transform);
				}
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, textureContainer);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, textureAwesomeface);
				glBindVertexArray(VAO[0]);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

				// Draw two triangles with the "container" texture and the "awesomeface" texture mixed
				// Scale them over time, and move them to the top left corner
				shaderTexture2xMix.use();
				shaderTexture2xMix.setBool("flip", flipped);
				shaderTexture2xMix.setFloat("offsetX", offsetX);
				shaderTexture2xMix.setFloat("offsetY", offsetY);
				shaderTexture2xMix.setFloat("mixWeight", textureMix);
				{
					mat4 transform(1.0f);
					transform = translate(transform, vec3(-0.5f, 0.5f, 0.0f));
					transform = scale(transform, ((sin((float)glfwGetTime()) / 2) + 0.5f) * vec3(1.0f, 1.0f, 1.0f));
					shaderTexture2xMix.setMat4f("transform", transform);
				}
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, textureContainer);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, textureAwesomeface);
				glBindVertexArray(VAO[0]);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
				break;
		}

		// Check and call events and swap buffers
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}