#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

using namespace std;


// Vertices and indices for triangles
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

// Vertex shader
const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

// Fragment shader
const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";

// Yellow fragment shader
const char *yellowFragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"	FragColor = vec4(1.0f, 1.0f, 0.2f, 1.0f);\n"
"}\0";

// State variables
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

	//-------------------------
	// Shader program (default)
	//-------------------------

	// Create and compile vertex shader
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// Create and compile fragment shader
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// Check whether shader compilations succeeded or not, print errors if not
	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
	}
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		cerr << "ERROR::SHADER::FRAGMENT_DEFAULT::COMPILATION_FAILED\n" << infoLog << endl;
	}

	// Create shader program, attach shaders, and link them
	unsigned int shaderProgramDefault = glCreateProgram();
	glAttachShader(shaderProgramDefault, vertexShader);
	glAttachShader(shaderProgramDefault, fragmentShader);
	glLinkProgram(shaderProgramDefault);

	// Check whether linking the shader program succeeded or not, print errors if not
	glGetProgramiv(shaderProgramDefault, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgramDefault, 512, NULL, infoLog);
		cerr << "ERROR::SHADER_PROGRAM_DEFAULT::LINKING_FAILED\n" << infoLog << endl;
	}

	// Delete the now redundant shader objects (only fragment, we're reusing vertex)
	glDeleteShader(vertexShader);

	//------------------------
	// Shader program (yellow)
	//------------------------

	// Create and compile fragment shader
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &yellowFragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// Check whether shader compilation succeeded or not, print errors if not
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		cerr << "ERROR::SHADER::FRAGMENT_YELLOW::COMPILATION_FAILED\n" << infoLog << endl;
	}

	// Create shader program, attach shaders, and link them
	unsigned int shaderProgramYellow = glCreateProgram();
	glAttachShader(shaderProgramYellow, vertexShader);
	glAttachShader(shaderProgramYellow, fragmentShader);
	glLinkProgram(shaderProgramYellow);

	// Check whether linking the shader program succeeded or not, print errors if not
	glGetProgramiv(shaderProgramYellow, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgramYellow, 512, NULL, infoLog);
		cerr << "ERROR::SHADER_PROGRAM_YELLOW::LINKING_FAILED\n" << infoLog << endl;
	}

	// Delete the now redundant shader objects
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

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
				glUseProgram(shaderProgramDefault);
				glBindVertexArray(VAO[0]);
				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
				glBindVertexArray(0);
				break;
			case 1:
				glUseProgram(shaderProgramDefault);
				glBindVertexArray(VAO[1]);
				glDrawArrays(GL_TRIANGLES, 0, 3);

				glUseProgram(shaderProgramYellow);
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