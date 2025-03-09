#include <fstream>
#include <sstream>
#include <iostream>
#include "shader.h"

using namespace std;
using namespace glm;


Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	//----------------------------------------------------------
	// 1. Retrieve the vertex/fragment source code from filePath
	//----------------------------------------------------------
	string vertexCode;
	string fragmentCode;
	ifstream vShaderFile;
	ifstream fShaderFile;
	// Ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	fShaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	try
	{
		// Open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		stringstream vShaderStream, fShaderStream;
		// Read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// Close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// Convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (ifstream::failure e)
	{
		cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	//-------------------
	// 2. Compile shaders
	//-------------------
	unsigned int vertex, fragment;
	int success;
	char infoLog[512];

	// Vertex Shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	// Print compile errors, if any
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
	};

	// Fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	// Print compile errors, if any
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
	};

	// Shader Program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	// Print linking errors if any
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
	}

	// Delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}


void Shader::use() const
{
	glUseProgram(ID);
}


// Uniform setter for bools
void Shader::setBool(const string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}


// Uniform setter for ints
void Shader::setInt(const string &name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}


// Uniform setter for floats
void Shader::setFloat(const string &name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}


// Uniform setter for vec3 (with floats, probably all we'll need)
void Shader::setVec3f(const string &name, vec3 value) const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, value_ptr(value));
}


// Uniform setter for vec4 (with floats, probably all we'll need)
void Shader::setVec4f(const string &name, vec4 value) const
{
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, value_ptr(value));
}


// Uniform setter for mat4 (with floats, probably all we'll need)
void Shader::setMat4f(const string &name, mat4 value) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, value_ptr(value));
}