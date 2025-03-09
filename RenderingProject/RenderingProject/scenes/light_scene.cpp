#include "light_scene.h"

using namespace std;
using namespace glm;


LightScene::LightScene(GLFWwindow *w, Camera *c)
{
	window = w;
	camera = c;


	//------------------
	// Generate textures
	//------------------

	// Nothing to generate for now


	//-----------------------------------------------
	// Generate shaders and set samplers for textures
	//-----------------------------------------------

	boxShader = Shader("shaders/vert_lightSceneLitObject.vs", "shaders/frag_lightSceneLitObject.fs");
	lightSourceShader = Shader("shaders/vert_lightSceneLightSource.vs", "shaders/frag_lightSceneLightSource.fs");


	//--------------
	// Setup buffers
	//--------------

	// --Box--

	glGenVertexArrays(1, &boxVAO);
	glBindVertexArray(boxVAO);

	glGenBuffers(1, &boxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices), boxVertices, GL_STATIC_DRAW);

	// aPos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	// aNormal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//----------


	// --Light--

	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	// Same as box
	glBindBuffer(GL_ARRAY_BUFFER, boxVBO);

	// aPos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
}


void LightScene::render()
{
	// Recalculate light position to animate the light source
	lightPos.x = sin((float)glfwGetTime()) * 2.0f;
	lightPos.y = 1.0f + sin((float)glfwGetTime() / 2.0f);

	// View matrix
	mat4 view(1.0f);
	view = camera->getViewMatrix();

	// Projection matrix
	mat4 projection(1.0f);
	int viewportW, viewportH;
	glfwGetFramebufferSize(window, &viewportW, &viewportH);
	projection = perspective(camera->fov, (float)viewportW / (float)viewportH, 0.1f, 100.0f);

	//-----------------
	// Render a lit box
	//-----------------

	boxShader.use();

	boxShader.setVec3f("objectColor", coralColor);
	boxShader.setVec3f("lightColor", whiteColor);
	boxShader.setVec3f("lightPos", lightPos);
	boxShader.setVec3f("viewPos", camera->position);

	// Model matrix for box, at world origin with no transformations
	mat4 boxModel(1.0f);
	boxShader.setMat4f("model", boxModel);

	// Normal matrix for box
	mat3 boxNormal(1.0f);
	boxNormal = mat3(transpose(inverse(view * boxModel)));
	boxShader.setMat3f("normalMatView", boxNormal);

	boxShader.setMat4f("view", view);
	boxShader.setMat4f("projection", projection);

	// Draw a box
	glBindVertexArray(boxVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	
	//----------------------
	// Render a light source
	//----------------------

	lightSourceShader.use();

	// Model matrix for light source, positioned at lightPos and scaled down
	mat4 lightModel(1.0f);
	lightModel = translate(lightModel, lightPos);
	lightModel = scale(lightModel, vec3(0.2f));
	lightSourceShader.setMat4f("model", lightModel);

	lightSourceShader.setMat4f("view", view);
	lightSourceShader.setMat4f("projection", projection);

	// Draw the light source
	glBindVertexArray(lightVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}


void LightScene::handleKey(int key)
{
	// Page up
	if (key == GLFW_KEY_PAGE_UP)
	{
		// No functionality implemented
	}

	// Page down
	if (key == GLFW_KEY_PAGE_DOWN)
	{
		// No functionality implemented
	}
}