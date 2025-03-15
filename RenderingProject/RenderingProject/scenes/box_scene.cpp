#include "box_scene.h"

using namespace std;
using namespace glm;


BoxScene::BoxScene(GLFWwindow *w, Camera *c)
{
	window = w;
	camera = c;


	//------------------
	// Generate textures
	//------------------

	containerTexture = Texture("textures/container.jpg", GL_REPEAT, GL_RGB, GL_RGB);
	faceTexture = Texture("textures/awesomeface.png", GL_REPEAT, GL_RGBA, GL_RGBA);


	//-----------------------------------------------
	// Generate shaders and set samplers for textures
	//-----------------------------------------------

	boxShader = Shader("shaders/vert_boxScene.vs", "shaders/frag_boxScene.fs");
	boxShader.use();
	boxShader.setInt("tex0", 0);
	boxShader.setInt("tex1", 1);


	//--------------
	// Setup buffers
	//--------------

	glGenVertexArrays(1, &boxVAO);
	glBindVertexArray(boxVAO);

	glGenBuffers(1, &boxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices), boxVertices, GL_STATIC_DRAW);

	// aPos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	// aTexCoord
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
}


void BoxScene::render()
{
	boxShader.use();
	boxShader.setFloat("mixWeight", textureMix);
	
	// View matrix
	mat4 view(1.0f);
	view = camera->getViewMatrix();
	boxShader.setMat4f("view", view);

	// Projection matrix
	mat4 projection(1.0f);
	int viewportW, viewportH;
	glfwGetFramebufferSize(window, &viewportW, &viewportH);
	projection = perspective(camera->fov, (float)viewportW / (float)viewportH, 0.1f, 100.0f);
	boxShader.setMat4f("projection", projection);
	
	glActiveTexture(GL_TEXTURE0);
	containerTexture.bind();
	glActiveTexture(GL_TEXTURE1);
	faceTexture.bind();
	glBindVertexArray(boxVAO);

	for (size_t i = 0; i < size(boxPositions); i++)
	{
		// Model matrix, different for each box to move them in world space
		mat4 model(1.0f);
		model = translate(model, boxPositions[i]);
		float angle = 20.0f * i;
		if (i % 3 == 0)
		{
			// Make every 3rd box spin
			angle += (float)glfwGetTime() * 50.0f;
		}
		model = rotate(model, radians(angle), vec3(1.0f, 0.3f, 0.5f));
		boxShader.setMat4f("model", model);

		// Draw a box
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}


void BoxScene::handleKey(int key, float deltaTime)
{
	// Page up
	if (key == GLFW_KEY_PAGE_UP)
	{
		textureMix += deltaTime;

		if (textureMix > 1.0f)
		{
			textureMix = 1.0f;
		}
	}

	// Page down
	if (key == GLFW_KEY_PAGE_DOWN)
	{
		textureMix -= deltaTime;

		if (textureMix < 0.0f)
		{
			textureMix = 0.0f;
		}
	}
}