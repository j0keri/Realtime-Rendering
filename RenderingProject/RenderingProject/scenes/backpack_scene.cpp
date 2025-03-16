#include "backpack_scene.h"

using namespace std;
using namespace glm;


BackpackScene::BackpackScene(GLFWwindow *w, Camera *c)
{
	window = w;
	camera = c;


	//------------
	// Load models
	//------------

	backpackModel = Model("models/backpack/backpack.obj");


	//-----------------------------------------------
	// Generate shaders and set samplers for textures
	//-----------------------------------------------

	// Samplers for textures are set by the model itself so no need to set any here now
	backpackShader = Shader("shaders/vert_lightSceneLitObject.vs", "shaders/frag_lightSceneLitObject.fs");
	lightSourceShader = Shader("shaders/vert_lightSceneLightSource.vs", "shaders/frag_lightSceneLightSource.fs");


	//--------------
	// Setup buffers
	//--------------

	// No need to setup buffers for the model as it does it by itself internally


	// --Light--

	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glGenBuffers(1, &lightVBO);
	glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lightVertices), lightVertices, GL_STATIC_DRAW);

	// aPos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
}


void BackpackScene::render()
{
	glClearColor(skyColor.x, skyColor.y, skyColor.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// View matrix
	mat4 view(1.0f);
	view = camera->getViewMatrix();

	// Projection matrix
	mat4 projection(1.0f);
	int viewportW, viewportH;
	glfwGetFramebufferSize(window, &viewportW, &viewportH);
	projection = perspective(camera->fov, (float)viewportW / (float)viewportH, 0.1f, 100.0f);

	//--------------------
	// Render the backpack
	//--------------------

	backpackShader.use();

	backpackShader.setFloat("material.shininess", 32.0f);
	backpackShader.setFloat("material.emissionIntensity", 0.0f);

	// Directional light properties
	backpackShader.setVec3f("directionalLight.direction", vec3(view * vec4(directionalLightDirection, 0.0f)));
	backpackShader.setVec3f("directionalLight.ambient", directionalLightColor * 0.1f);
	backpackShader.setVec3f("directionalLight.diffuse", directionalLightColor);
	backpackShader.setVec3f("directionalLight.specular", directionalLightSpecular);

	// Point light properties
	for (size_t i = 0; i < size(pointLightPositions); i++)
	{
		backpackShader.setVec3f("pointLights[" + to_string(i) + "].position", vec3(view * vec4(pointLightPositions[i], 1.0f)));
		backpackShader.setVec3f("pointLights[" + to_string(i) + "].ambient", pointLightColors[i] * 0.1f);
		backpackShader.setVec3f("pointLights[" + to_string(i) + "].diffuse", pointLightColors[i]);
		backpackShader.setVec3f("pointLights[" + to_string(i) + "].specular", pointLightSpeculars[i]);
		backpackShader.setFloat("pointLights[" + to_string(i) + "].constant", 1.0f);
		backpackShader.setFloat("pointLights[" + to_string(i) + "].linear", 0.09f);
		backpackShader.setFloat("pointLights[" + to_string(i) + "].quadratic", 0.032f);
	}

	// Spotlight properties
	backpackShader.setVec3f("spotLight.position", vec3(0.0f));
	backpackShader.setVec3f("spotLight.direction", vec3(0.0f, 0.0f, -1.0f));
	if (flashlight)
	{
		backpackShader.setVec3f("spotLight.ambient", spotLightColor * 0.1f);
		backpackShader.setVec3f("spotLight.diffuse", spotLightColor);
		backpackShader.setVec3f("spotLight.specular", spotLightSpecular);
	}
	else
	{
		backpackShader.setVec3f("spotLight.ambient", vec3(0.0f));
		backpackShader.setVec3f("spotLight.diffuse", vec3(0.0f));
		backpackShader.setVec3f("spotLight.specular", vec3(0.0f));
	}
	backpackShader.setFloat("spotLight.constant", 1.0f);
	backpackShader.setFloat("spotLight.linear", 0.09f);
	backpackShader.setFloat("spotLight.quadratic", 0.032f);
	backpackShader.setFloat("spotLight.innerCutOff", cos(radians(spotLightInnerCutOff)));
	backpackShader.setFloat("spotLight.outerCutOff", cos(radians(spotLightOuterCutOff)));

	backpackShader.setVec3f("viewPos", camera->position);

	// Model matrix for the backpack
	mat4 backpackModelMat = mat4(1.0f);
	backpackModelMat = translate(backpackModelMat, vec3(0.0f, 0.0f, 0.0f));
	backpackModelMat = scale(backpackModelMat, vec3(1.0f, 1.0f, 1.0f));

	// Normal matrix for backpack
	mat3 backpackNormal(1.0f);
	backpackNormal = mat3(transpose(inverse(view * backpackModelMat)));

	backpackShader.setMat4f("model", backpackModelMat);
	backpackShader.setMat4f("view", view);
	backpackShader.setMat4f("projection", projection);
	backpackShader.setMat3f("normalMatView", backpackNormal);

	// Draw the model, with the shader properties we set above
	backpackModel.draw(backpackShader);


	//---------------------
	// Render light sources
	//---------------------

	lightSourceShader.use();

	lightSourceShader.setMat4f("view", view);
	lightSourceShader.setMat4f("projection", projection);

	glBindVertexArray(lightVAO);

	// Draw light sources
	for (size_t i = 0; i < size(pointLightPositions); i++)
	{
		lightSourceShader.setVec3f("lightColor", pointLightColors[i]);

		// Model matrix for light source
		mat4 lightModelMat(1.0f);
		lightModelMat = translate(lightModelMat, pointLightPositions[i]);
		lightModelMat = scale(lightModelMat, vec3(0.2f));
		lightSourceShader.setMat4f("model", lightModelMat);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}


void BackpackScene::handleKey(int key, float deltaTime)
{
	// Up arrow
	if (key == GLFW_KEY_UP)
	{
		lightingScheme = (lightingScheme + 1) % amountSchemes;
		adjustLights();
	}

	// Down arrow
	if (key == GLFW_KEY_DOWN)
	{
		lightingScheme--;
		if (lightingScheme < 0)
		{
			lightingScheme = amountSchemes - 1;
		}
		adjustLights();
	}

	// F
	if (key == GLFW_KEY_F)
	{
		flashlight = !flashlight;
	}

	// Page up
	if (key == GLFW_KEY_PAGE_UP)
	{
		// Nothing implemented
	}

	// Page down
	if (key == GLFW_KEY_PAGE_DOWN)
	{
		// Nothing implemented
	}
}


void BackpackScene::adjustLights()
{
	switch (lightingScheme)
	{
		// Default
	case 0:
		skyColor = vec3(0.05f, 0.05f, 0.1f);

		directionalLightColor = vec3(0.5f);
		directionalLightSpecular = vec3(1.0f);

		pointLightColors[0] = vec3(0.5f);
		pointLightColors[1] = vec3(0.5f);
		pointLightColors[2] = vec3(0.5f);
		pointLightColors[3] = vec3(0.5f);

		pointLightSpeculars[0] = vec3(1.0f);
		pointLightSpeculars[1] = vec3(1.0f);
		pointLightSpeculars[2] = vec3(1.0f);
		pointLightSpeculars[3] = vec3(1.0f);
		break;
		// Desert
	case 1:
		skyColor = vec3(0.7f, 0.5f, 0.3f);

		directionalLightColor = vec3(0.8f, 0.7f, 0.4f);
		directionalLightSpecular = vec3(1.0f);

		pointLightColors[0] = vec3(1.0f, 0.3f, 0.0f);
		pointLightColors[1] = vec3(1.0f, 0.0f, 0.0f);
		pointLightColors[2] = vec3(1.0f, 0.3f, 0.0f);
		pointLightColors[3] = vec3(0.8f, 0.5f, 0.0f);

		pointLightSpeculars[0] = vec3(1.0f);
		pointLightSpeculars[1] = vec3(1.0f);
		pointLightSpeculars[2] = vec3(1.0f);
		pointLightSpeculars[3] = vec3(1.0f);
		break;
		// Factory
	case 2:
		skyColor = vec3(0.1f, 0.1f, 0.1f);

		directionalLightColor = vec3(0.2f, 0.1f, 0.5f);
		directionalLightSpecular = vec3(0.5f);

		pointLightColors[0] = vec3(0.1f, 0.1f, 0.3f);
		pointLightColors[1] = vec3(0.1f, 0.1f, 0.3f);
		pointLightColors[2] = vec3(0.1f, 0.1f, 0.3f);
		pointLightColors[3] = vec3(0.1f, 0.1f, 0.3f);

		pointLightSpeculars[0] = vec3(1.0f);
		pointLightSpeculars[1] = vec3(1.0f);
		pointLightSpeculars[2] = vec3(1.0f);
		pointLightSpeculars[3] = vec3(1.0f);
		break;
		// Horror
	case 3:
		skyColor = vec3(0.0f, 0.0f, 0.0f);

		directionalLightColor = vec3(0.01f, 0.01f, 0.01f);
		directionalLightSpecular = vec3(0.0f);

		pointLightColors[0] = vec3(0.1f, 0.025f, 0.0f);
		pointLightColors[1] = vec3(0.1f, 0.025f, 0.0f);
		pointLightColors[2] = vec3(0.1f, 0.025f, 0.0f);
		pointLightColors[3] = vec3(0.1f, 0.025f, 0.0f);

		pointLightSpeculars[0] = vec3(0.4f, 0.1f, 0.0f);
		pointLightSpeculars[1] = vec3(0.4f, 0.1f, 0.0f);
		pointLightSpeculars[2] = vec3(0.4f, 0.1f, 0.0f);
		pointLightSpeculars[3] = vec3(0.4f, 0.1f, 0.0f);
		break;
		// Biochemical lab
	case 4:
		skyColor = vec3(0.95f, 1.0f, 0.95f);

		directionalLightColor = vec3(0.5f, 0.7f, 0.5f);
		directionalLightSpecular = vec3(1.0f);

		pointLightColors[0] = vec3(0.4f, 0.7f, 0.4f);
		pointLightColors[1] = vec3(0.4f, 0.7f, 0.4f);
		pointLightColors[2] = vec3(0.4f, 0.7f, 0.4f);
		pointLightColors[3] = vec3(0.4f, 0.7f, 0.4f);

		pointLightSpeculars[0] = vec3(1.0f);
		pointLightSpeculars[1] = vec3(1.0f);
		pointLightSpeculars[2] = vec3(1.0f);
		pointLightSpeculars[3] = vec3(1.0f);
		break;
	}
}