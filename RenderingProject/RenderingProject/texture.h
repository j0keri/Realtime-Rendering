#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <stb_image.h>
#include <iostream>


class Texture
{
public:
	// Texture ID
	GLuint ID;

	// Constructor to generate texture from image
	Texture(const char* imagePath, GLint wrapMode, GLint internalformat, GLenum format);

	// Default constructor
	Texture() = default;

	// Bind texture
	void bind();
};

#endif