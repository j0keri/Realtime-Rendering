#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <iostream>
#include <string>
#include <vector>
#include "mesh.h"
#include "shader.h"


class Model
{
public:
	// Constructor
	Model(const char *path);

	// Default constructor
	Model() = default;

	// Draw the model
	void draw(Shader &shader) const;


private:
	// Model data
	std::vector<Mesh> meshes;
	std::string directory;

	// Vector of all loaded textures so duplicates don't have to be loaded
	std::vector<Texture> textures_loaded;

	// Load model data
	void loadModel(std::string path);

	// Process a node from the model, then recurively process all its child nodes
	void processNode(aiNode *node, const aiScene *scene);

	// Process a mesh from the model into an instance of our own mesh class
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);

	// Helper function to retrieve, load, and initialize the textures from a given material
	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);

	// Helper function to load texture images from file
	GLuint textureFromFile(const char *path, const std::string &directory);
};

#endif