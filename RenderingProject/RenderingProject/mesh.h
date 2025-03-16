#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <string>
#include <vector>
#include "shader.h"


struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};


struct Texture {
	GLuint ID;
	std::string type;
	std::string path;
};


class Mesh
{
public:
	// Mesh data
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	// Constructor
	Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);

	// Draw the mesh
	void draw(Shader &shader) const;


private:
	// Render data
	GLuint VAO, VBO, EBO;

	// Initialize buffers for drawing the mesh
	void setupMesh();
};

#endif