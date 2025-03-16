#include "texture_legacy.h"

using namespace std;


TextureLegacy::TextureLegacy(const char* imagePath, GLint wrapMode)
{
	// Flip texture images vertically
	stbi_set_flip_vertically_on_load(true);

	// Generate texture
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);

	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Load texture image
	int width, height, nrChannels;
	unsigned char *data = stbi_load(imagePath, &width, &height, &nrChannels, 0);

	// Assign image to texture and generate mipmaps
	if (data)
	{
		GLenum format;
		if (nrChannels == 1)
		{
			format = GL_RED;
		}
		else if (nrChannels == 3)
		{
			format = GL_RGB;
		}
		else if (nrChannels == 4)
		{
			format = GL_RGBA;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		cerr << "Failed to load texture" << endl;
	}

	// Free the image memory
	stbi_image_free(data);
}


void TextureLegacy::bind()
{
	glBindTexture(GL_TEXTURE_2D, ID);
}