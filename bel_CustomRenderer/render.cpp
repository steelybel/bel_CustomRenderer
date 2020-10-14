#include "render.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

geometry makeGeometry(vertex * verts, size_t vertCount, unsigned int * indices, size_t indxCount)
{
	//make instance geometry
	geometry newGeo = {};
	newGeo.size = indxCount;
	//generate buffers and VAO
	glGenBuffers(1, &newGeo.vbo);
	glGenBuffers(1, &newGeo.ibo);
	glGenVertexArrays(1, &newGeo.vao);


	//bind VAO and buffer
	glBindVertexArray(newGeo.vao);
	glBindBuffer(GL_ARRAY_BUFFER, newGeo.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newGeo.ibo);

	//order matters!

	//populate buffers
	glBufferData(GL_ARRAY_BUFFER, vertCount * sizeof(vertex), verts, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indxCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	//describe the data contained within the buffers
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,
		4,
		GL_FLOAT,
		GL_FALSE,
		sizeof(vertex),
		(void*)0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//return the geo
	return newGeo;
}

void freeGeometry(geometry & geo)
{
	glDeleteBuffers(1, &geo.vbo);
	glDeleteBuffers(1, &geo.ibo);
	glDeleteVertexArrays(1, &geo.vao);

	//deleted
	geo = {};
}

texture loadTexture(const char * filePath)
{
	assert(filePath != nullptr && "File path was invalid");

	//variables to hold onto some dataa
	int imageWidth, imageHeight, imageFormat;
	unsigned char* rawPixelData = nullptr;

	//load image using stb
	stbi_set_flip_vertically_on_load(true);
	rawPixelData = stbi_load(filePath,
		&imageWidth,
		&imageHeight,
		&imageFormat,
		STBI_default);
	assert(rawPixelData != nullptr && "Image failed to load.");
	
	//pass image data to makeTexture
	texture newTexture = makeTexture(imageWidth, imageHeight, imageFormat, rawPixelData);
	assert(newTexture.handle != 0 && "Failed to create texture.");
	stbi_image_free(rawPixelData);

	// return the texture
	return newTexture;
}

texture makeTexture(unsigned width, unsigned height, unsigned channels, const unsigned char * pixels)
{
	// figure out the OGL texture format to use
	assert(channels > 0 && channels < 5); // only support 1-4 channels
	GLenum oglFormat = GL_RED;
	switch (channels)
	{
	case 1:
		oglFormat = GL_RED;     // RED
		break;
	case 2:
		oglFormat = GL_RG;      // RED GREEN
		break;
	case 3:
		oglFormat = GL_RGB;     // RED GREEN BLUE
		break;
	case 4:
		oglFormat = GL_RGBA;    // RED GREEN BLUE ALPHA
		break;
	default:
		fprintf(stderr, "Invalid format specified! Must be R/G/B/A.");
		break;
	}

	// generate a texture
	texture retVal = { 0, width, height, channels };
	glGenTextures(1, &retVal.handle);

	// bind and buffer data to it
	glBindTexture(GL_TEXTURE_2D, retVal.handle);

	glTexImage2D(GL_TEXTURE_2D,         // texture to buffer data to (was previously bound)
		0,                     // level
		oglFormat,             // format specifier
		width,                 // width in pixels
		height,                // height in pixels
		0,                     // border value
		oglFormat,             // final format specifier
		GL_UNSIGNED_BYTE,      // type of data elements
		pixels);               // pointer to actual data

// set some filtering settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);  // filtering applied when texel density is greater than display (interpolating existing data)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  // filtering applied when texel density is less than display (extrapolating data)

	// return the texture
	glBindTexture(GL_TEXTURE_2D, 0);

	return retVal;
}

void freeTexture(texture & tex)
{
	glDeleteTextures(1, &tex.handle);
	tex = {};   // default initialize
}

bool checkShader(GLuint target, const char * humanReadableName = "")
{
	assert(glIsShader(target) && "target is not a shader -- cannot check");

	GLint shaderCompileStatus = 0;
	glGetShaderiv(target, GL_COMPILE_STATUS, &shaderCompileStatus);
	if (shaderCompileStatus != GL_TRUE)
	{
		GLsizei logLength = 0;
		GLchar message[1024];
		glGetShaderInfoLog(target, 1024, &logLength, message);
		fprintf(stderr, "\n[ERROR] %s Shader \n %s", humanReadableName, message);

		// return an empty shader if it fails
		return false;
	}

	return true;
}

shader makeShader(const char * vertSource, const char * fragSource)
{
	// make a shader program
	shader newShad = {};
	newShad.program = glCreateProgram(); // no parameters

	// create the shaders (not the same thing as a shader program)
	GLuint vert = glCreateShader(GL_VERTEX_SHADER);     // this shader is a vertex shader
	GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);   // this shader is a fragment shader

	// compile the shaders
	glShaderSource(vert, 1, &vertSource, 0);    // send source code for specified shader
	glShaderSource(frag, 1, &fragSource, 0);
	glCompileShader(vert);                      // actually compile the shader
	glCompileShader(frag);

	// validate the shaders
	bool shaderOK = checkShader(vert, "Vertex");
	if (!shaderOK)
	{
		return newShad;
	}
	shaderOK = checkShader(frag, "Fragment");
	if (!shaderOK)
	{
		return newShad;
	}

	// attach the shaders
	glAttachShader(newShad.program, vert); // associate the vertex shader w/ the shader program
	glAttachShader(newShad.program, frag); // associate the fragment shader w/ the shader program

	// link the shader program
	glLinkProgram(newShad.program); // link the shader program

	// delete the shaders
	glDeleteShader(vert);   // delete the vertex shader
	glDeleteShader(frag);   // delete the fragment shader

	// return the shader
	return newShad;
}

void freeShader(shader & shad)
{
	glDeleteProgram(shad.program);
	shad = {};
}

void draw(const shader & shad, const geometry & geo)
{
	// specify which shader
	glUseProgram(shad.program);
	// specify which VAO
	glBindVertexArray(geo.vao);

	// draw
	glDrawElements(GL_TRIANGLES,    // primitive type
		geo.size,        // indices
		GL_UNSIGNED_INT, // index type
		0);
}
