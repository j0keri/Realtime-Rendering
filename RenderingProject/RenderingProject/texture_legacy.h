#ifndef TEXTURE_LEGACY_H
#define TEXTURE_LEGACY_H

#include <glad/glad.h>
#include <stb_image.h>
#include <iostream>


class TextureLegacy
{
public:
	// Texture ID
	GLuint ID;

	// Constructor to generate texture from image
	TextureLegacy(const char* imagePath, GLint wrapMode);

	// Default constructor
	TextureLegacy() = default;

	// Bind texture
	void bind();
};

#endif