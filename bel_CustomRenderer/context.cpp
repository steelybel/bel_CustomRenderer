#include "context.h" //header to be implemented

//system libraries
#include <cstdio>
#include <iostream>
#include <cassert>

//project libraries
#include "glew/GL/glew.h"    // always before glfw.
#include "glfw/glfw3.h"

void APIENTRY errorCallback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLuint severity,
	GLuint length,
	const GLchar* message,
	const void* userParam)
{
	//std::cerr << message << std::endl;
	fprintf(stderr, message);
}

bool context::init(int width, int height, const char * title)
{
	//initialize glfw
	int glfwStatus = glfwInit();

	//test 4 errors
	if (glfwStatus == GLFW_FALSE)
	{
		printf("Error occurred with GLFW.");
		return false;
	}

	assert(glfwStatus != GLFW_FALSE && "An error occurred with GLFW.");

	window = glfwCreateWindow(width, height, title, nullptr, nullptr);

	glfwMakeContextCurrent(window);

	//initialize glew
	glewInit();

	//print out some diagnostics
	// GL_VERSION
	printf("OpenGL ver.: %s/n", (const char*)glGetString(GL_VERSION));
	// GL_RENDERER
	printf("Renderer: %s/n", (const char*)glGetString(GL_RENDERER));
	// GL_VENDOR
	printf("Vendor: %s/n", (const char*)glGetString(GL_VENDOR));
	// GL_SHADING_LANGUAGE_VERSION
	printf("GLSL: %s/n", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
	//set up some good defaults
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
	
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);


	//return true if everything's OK
	return true;
}

void context::tick()
{
	glfwPollEvents();
	glfwSwapBuffers(window);
}

void context::term()
{
	glfwDestroyWindow(window);
	window = nullptr;

	//clean up GLFW (also terminates GLEW!)
	glfwTerminate();
}

void context::clear()
{
	//clears replaces with color given by function call
	glClear(GL_COLOR_BUFFER_BIT);
}

bool context::shouldClose() const
{
	return glfwWindowShouldClose(window);
}
