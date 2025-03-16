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

	containerDiffuseMap = TextureLegacy("textures/container2.png", GL_REPEAT);
	containerSpecularMap = TextureLegacy("textures/container2_specular.png", GL_REPEAT);
	containerEmissionMap = TextureLegacy("textures/matrix.jpg", GL_REPEAT);


	//-----------------------------------------------
	// Generate shaders and set samplers for textures
	//-----------------------------------------------

	boxShader = Shader("shaders/vert_lightSceneLitObject.vs", "shaders/frag_lightSceneLitObject.fs");
	boxShader.use();
	boxShader.setInt("material.texture_diffuse1", 0);
	boxShader.setInt("material.texture_specular1", 1);
	boxShader.setInt("material.texture_emission1", 2);

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);

	// aNormal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	// aTexCoords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	//----------


	// --Light--

	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	// Same as box
	glBindBuffer(GL_ARRAY_BUFFER, boxVBO);

	// aPos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
}


void LightScene::render()
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

	//-----------------
	// Render lit boxes
	//-----------------

	boxShader.use();

	boxShader.setFloat("material.shininess", 32.0f);
	boxShader.setFloat("material.emissionIntensity", emissionIntensity);

	// Directional light properties
	boxShader.setVec3f("directionalLight.direction", vec3(view * vec4(directionalLightDirection, 0.0f)));
	boxShader.setVec3f("directionalLight.ambient", directionalLightColor * 0.1f);
	boxShader.setVec3f("directionalLight.diffuse", directionalLightColor);
	boxShader.setVec3f("directionalLight.specular", directionalLightSpecular);

	// Point light properties
	for (size_t i = 0; i < size(pointLightPositions); i++)
	{
		boxShader.setVec3f("pointLights[" + to_string(i) + "].position", vec3(view * vec4(pointLightPositions[i], 1.0f)));
		boxShader.setVec3f("pointLights[" + to_string(i) + "].ambient", pointLightColors[i] * 0.1f);
		boxShader.setVec3f("pointLights[" + to_string(i) + "].diffuse", pointLightColors[i]);
		boxShader.setVec3f("pointLights[" + to_string(i) + "].specular", pointLightSpeculars[i]);
		boxShader.setFloat("pointLights[" + to_string(i) + "].constant", 1.0f);
		boxShader.setFloat("pointLights[" + to_string(i) + "].linear", 0.09f);
		boxShader.setFloat("pointLights[" + to_string(i) + "].quadratic", 0.032f);
	}

	// Spotlight properties
	boxShader.setVec3f("spotLight.position", vec3(0.0f));
	boxShader.setVec3f("spotLight.direction", vec3(0.0f, 0.0f, -1.0f));
	if (flashlight)
	{
		boxShader.setVec3f("spotLight.ambient", spotLightColor * 0.1f);
		boxShader.setVec3f("spotLight.diffuse", spotLightColor);
		boxShader.setVec3f("spotLight.specular", spotLightSpecular);
	}
	else
	{
		boxShader.setVec3f("spotLight.ambient", vec3(0.0f));
		boxShader.setVec3f("spotLight.diffuse", vec3(0.0f));
		boxShader.setVec3f("spotLight.specular", vec3(0.0f));
	}
	boxShader.setFloat("spotLight.constant", 1.0f);
	boxShader.setFloat("spotLight.linear", 0.09f);
	boxShader.setFloat("spotLight.quadratic", 0.032f);
	boxShader.setFloat("spotLight.innerCutOff", cos(radians(spotLightInnerCutOff)));
	boxShader.setFloat("spotLight.outerCutOff", cos(radians(spotLightOuterCutOff)));

	boxShader.setVec3f("viewPos", camera->position);

	boxShader.setMat4f("view", view);
	boxShader.setMat4f("projection", projection);

	glActiveTexture(GL_TEXTURE0);
	containerDiffuseMap.bind();
	glActiveTexture(GL_TEXTURE1);
	containerSpecularMap.bind();
	glActiveTexture(GL_TEXTURE2);
	containerEmissionMap.bind();
	glBindVertexArray(boxVAO);

	// Draw boxes
	for (size_t i = 0; i < size(boxPositions); i++)
	{
		// Model matrix for box
		mat4 boxModel(1.0f);
		boxModel = translate(boxModel, boxPositions[i]);
		float angle = 20.0f * i;
		boxModel = rotate(boxModel, radians(angle), vec3(1.0f, 0.3f, 0.5f));
		boxShader.setMat4f("model", boxModel);

		// Normal matrix for box
		mat3 boxNormal(1.0f);
		boxNormal = mat3(transpose(inverse(view * boxModel)));
		boxShader.setMat3f("normalMatView", boxNormal);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	
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
		mat4 lightModel(1.0f);
		lightModel = translate(lightModel, pointLightPositions[i]);
		lightModel = scale(lightModel, vec3(0.2f));
		lightSourceShader.setMat4f("model", lightModel);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
}


void LightScene::handleKey(int key, float deltaTime)
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
		emissionIntensity += 2.0f * deltaTime;

		if (emissionIntensity > 2.0f)
		{
			emissionIntensity = 2.0f;
		}
	}

	// Page down
	if (key == GLFW_KEY_PAGE_DOWN)
	{
		emissionIntensity -= 2.0f * deltaTime;

		if (emissionIntensity < 0.0f)
		{
			emissionIntensity = 0.0f;
		}
	}
}


void LightScene::adjustLights()
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