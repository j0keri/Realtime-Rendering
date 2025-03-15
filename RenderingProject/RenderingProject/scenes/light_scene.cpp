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

	containerDiffuseMap = Texture("textures/container2.png", GL_REPEAT, GL_RGBA, GL_RGBA);
	containerSpecularMap = Texture("textures/container2_specular.png", GL_REPEAT, GL_RGBA, GL_RGBA);
	containerEmissionMap = Texture("textures/matrix.jpg", GL_REPEAT, GL_RGB, GL_RGB);


	//-----------------------------------------------
	// Generate shaders and set samplers for textures
	//-----------------------------------------------

	boxShader = Shader("shaders/vert_lightSceneLitObject.vs", "shaders/frag_lightSceneLitObject.fs");
	boxShader.use();
	boxShader.setInt("material.diffuse", 0);
	boxShader.setInt("material.specular", 1);
	boxShader.setInt("material.emission", 2);

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
	// Light position
	vec3 lightPos = lightPosDefault;
	if (false) // TODO: trigger with keyboard
	{
		// Recalculate light position to animate the light source
		lightPos.x = sin((float)glfwGetTime()) * 2.0f;
		lightPos.y = 1.0f + sin((float)glfwGetTime() / 2.0f);
	}
	
	// Light color
	vec3 lightColor = lightColorDefault;
	if (false) // TODO: trigger with keyboard
	{
		// Recalculate light color to animate lighting conditions
		lightColor.x = (sin(glfwGetTime() * 2.0f) / 2) + 0.5f;
		lightColor.y = (sin(glfwGetTime() * 0.7f) / 2) + 0.5f;
		lightColor.z = (sin(glfwGetTime() * 1.3f) / 2) + 0.5f;
	}
	vec3 lightDiffuse = lightColor * 0.5f;
	vec3 lightAmbient = lightDiffuse * 0.2f;

	// View matrix
	mat4 view(1.0f);
	view = camera->getViewMatrix();

	// Projection matrix
	mat4 projection(1.0f);
	int viewportW, viewportH;
	glfwGetFramebufferSize(window, &viewportW, &viewportH);
	projection = perspective(camera->fov, (float)viewportW / (float)viewportH, 0.1f, 100.0f);

	//-------------------
	// Render a lit boxes
	//-------------------

	boxShader.use();

	boxShader.setVec3f("material.ambient", coralColor);
	boxShader.setVec3f("material.diffuse", coralColor);
	boxShader.setVec3f("material.specular", vec3(0.5f));
	boxShader.setFloat("material.shininess", 32.0f);

	//boxShader.setVec3f("light.position", vec3(view * vec4(lightPos, 1.0f)));
	//boxShader.setVec3f("light.direction", vec3(view * vec4(directionalLightDir, 0.0f)));
	boxShader.setVec3f("light.position", vec3(0.0f));
	boxShader.setVec3f("light.direction", vec3(0.0f, 0.0f, -1.0f));
	boxShader.setFloat("light.innerCutOff", cos(radians(12.5f)));
	boxShader.setFloat("light.outerCutOff", cos(radians(17.5f)));

	boxShader.setVec3f("light.ambient", lightAmbient);
	boxShader.setVec3f("light.diffuse", lightDiffuse);
	boxShader.setVec3f("light.specular", vec3(1.0f));

	boxShader.setFloat("light.constant", 1.0f);
	boxShader.setFloat("light.linear", 0.09f);
	boxShader.setFloat("light.quadratic", 0.032f);

	boxShader.setVec3f("viewPos", camera->position);

	boxShader.setFloat("emissionIntensity", emissionIntensity);

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
		// Model matrix for box, different for each box to move them in world space
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

	
	//----------------------
	// Render a light source
	//----------------------

	lightSourceShader.use();

	lightSourceShader.setVec3f("lightColor", lightColor);

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


void LightScene::handleKey(int key, float deltaTime)
{
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