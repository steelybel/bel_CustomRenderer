#include "context.h"
#include "render.h"		// vertex, geometry, shader

int main()
{
	context game;
	game.init(256, 256, "cute bel");

	// - HERE'S ALL THE BIG STUFF

	// vertices (remember that work in unity?)
	vertex triVerts[] =
	{
		{ { -.5f, -.5f, 0, 1 } }, // bottom-left
		{ {  .5f, -.5f, 0, 1 } }, // bottom-right
		{ {   0.f, .5f, 0, 1 } }  // top-center
	};

	vertex quadVerts[] =
	{
		{ { -1.f, -1.f, 0, 1 } }, // bottom-left
		{ {   1.f,-1.f, 0, 1 } }, // bottom-right
		{ {  -1.f, 1.f, 0, 1 } }, // top-left
		{ {   1.f, 1.f, 0, 1 } }  // top-right
	};

	// indices (remember that work in unity?)
	//		   don't forget about the winding order
	unsigned int triIndices[] = { 2, 0, 1 };
	unsigned int quadIndices[] = { 3, 2, 0, 0, 1, 3 };

	// make the geometry
	geometry triangle = makeGeometry(triVerts, 3, triIndices, 3);
	geometry quad = makeGeometry(quadVerts, 4, quadIndices, 6);

	// source for vertex shader
	const char* basicVertShader =
		"#version 410\n"
		"layout (location = 0) in vec4 position;\n"	// in from vertex data
		"out vec4 vColor;\n"						// output to later stages
		"void main()\n"
		"{\n"
		"	gl_Position = position;\n"
		"	vColor = position;\n"
		"};";

	// source for fragment shader
	const char* basicFragShader =
		"#version 410\n"
		"in vec4 vColor;\n"			// input from prior stages (like the vertex shader)
		"out vec4 outputColor;\n"	// output to the framebuffer (your only output)
		"void main() { outputColor = vColor; }";

	// make the shader
	shader basicShader = makeShader(basicVertShader, basicFragShader);
	

	//GAME LOOP

	while (!game.shouldClose())
	{
		game.tick();

		game.clear();

	}
	return 0;
}