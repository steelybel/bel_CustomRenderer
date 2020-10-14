#pragma once

#include <vector>

#include "glew/GL/glew.h"
#include "glm/glm.hpp"
#include "tinyobjloader/tiny_obj_loader.h"

struct vertex
{
	glm::vec4 pos;
};

struct geometry
{
	GLuint vao, vbo, ibo;
	GLuint size;
};

struct shader
{
	GLuint program;
};

struct texture
{
	GLuint handle;
	unsigned width, height, channels;
};

//functions to make and unmake the above types
geometry makeGeometry(vertex* verts, size_t vertCount, unsigned int* indices, size_t indxCount);

void freeGeometry(geometry &geo);

texture loadTexture(const char* filePath);
texture makeTexture(unsigned width, unsigned height, unsigned channels, const unsigned char *pixels);
void freeTexture(texture &tex);

shader makeShader(const char *vertSource, const char *fragSource);

void freeShader(shader &shad);

void draw(const shader& shad, const geometry& geo);