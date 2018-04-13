/*
 * Copyright (c) 2018 Brendan Barnes
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"
#include "VertexBufferLayout.h"

static const unsigned int WIDTH = 1080;
static const unsigned int HEIGHT = 1080;

vmath::mat4 perspective;
vmath::mat4 translation;
vmath::mat4 rotation;
vmath::mat4 view;

float dist = 3.0f;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	dist += (float)yoffset / 2.0f;
	if (dist < 0.25f)
	{
		dist = 0.25f;
	}
}


vmath::vec4 mix(vmath::vec4 c1, vmath::vec4 c2, float p)
{
	return p * (c1 - c2) + c2;
}

void invert(vmath::vec4& c)
{
	c[0] = 1 - c[0];
	c[1] = 1 - c[1];
	c[2] = 1 - c[2];
	c[3] = 1 - c[3];
}

int main(void)
{
	GLFWwindow* window;
	
	/* Initialize the library */
	if (!glfwInit())
	{
		return -1;
	}
	glfwWindowHint(GLFW_SAMPLES, 16);
	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(WIDTH, HEIGHT, "Hello, Universe!", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	glfwSetScrollCallback(window, scroll_callback);
	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
	{
		return -1;
	}

	//-----DATA-----//
	float dt = 0.0005f;
	const unsigned int count = 10000;
	vmath::vec3* initial_points = new vmath::vec3[2*count];
	for (int i = 0; i < count; i++)
	{
		// Sphere
		/*float u = vmath::random<float>() + vmath::random<float>();
		float r;
		if (u > 1.0f)
		{
			r = 2.0f - u;
		}
		else
		{
			r = u;
		}
		vmath::vec3 dir = vmath::vec3::random() - vmath::vec3(0.5f);
		vmath::vec3 position = r*dir / length(dir);
		*/
		// Torus
		float R = 0.5f;
		float r = 0.15f;
		float u = vmath::random<float>();
		float v = vmath::random<float>();
		float w = vmath::random<float>();
		float theta = 2.0f*(float)M_PI*u;
		float phi = 2.0f*(float)M_PI*v;
		vmath::vec3 position = vmath::vec3( (R+r*cos(theta))*cos(phi), (R + r * cos(theta))*sin(phi), r*sin(theta));
		initial_points[2*i] = position;
		initial_points[2*i+1] = vmath::vec3(0.0f);
		if (w > (R + r*cos(theta))/(R + r) )
		{
			i--;
		}
	}
	//--------------//
{
	// Renderer
	Renderer renderer;
	//--------------//

	// Vertex Array
	VertexBuffer vb(initial_points, sizeof(vmath::vec3) * 2 * count);
	VertexBufferLayout layout;
	layout.Push(3);
	layout.Push(3);
	VertexArray va;
	va.AddBuffer(vb, layout);
	//--------------//

	// Transform feedback array
	VertexBuffer tvb(initial_points, sizeof(vmath::vec3) * 2 * count);
	VertexArray tva;
	tva.AddBuffer(tvb, layout);
	//--------------//

	delete[] initial_points;

	// Texture Buffer
	GLuint tb[2];
	glGenTextures(2, tb);
	glBindTexture(GL_TEXTURE_BUFFER, tb[0]);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, vb.GetID());
	glBindTexture(GL_TEXTURE_BUFFER, tb[1]);
	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, tvb.GetID());

	// Shader
	Shader shader("res/shaders/Basic.glsl");
	shader.Bind();
	vmath::vec4 color = {0.0f, 0.0f, 0.0f, 1.0f};
	shader.SetUniform4fv("uColor", color);
	static const char* vNames[] =
	{
		"newPosition",
		"newVelocity"
	};
	glTransformFeedbackVaryings(shader.GetProgramID(), 2, vNames, GL_INTERLEAVED_ATTRIBS);
	glLinkProgram(shader.GetProgramID());
	//--------------//
	
	//glPointSize(2.0f);
	shader.Unbind();
	va.Unbind();
	tva.Unbind();
	shader.Bind();
	unsigned int frame = 1;

	

	int statePlus = glfwGetKey(window, GLFW_KEY_KP_ADD);
	int stateMinus = glfwGetKey(window, GLFW_KEY_KP_SUBTRACT);

	

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		renderer.Clear();
		glClearBufferfv(GL_COLOR, 0, vmath::vec4(0.15f, 0.15f, 0.15f, 1.0f));
		
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		xpos = 2.0*xpos / (double)WIDTH - 1.0;
		ypos = -2.0*ypos / (double)HEIGHT + 1.0;
		float t = glfwGetTime()/4.0f;
		//xpos = cos(t)*0.7f;
		//ypos = cos(t)*sin(t)*0.7f;

		perspective = vmath::perspective(60.0, 1.0, 0.0, 2.0);
		translation = vmath::translate(vmath::vec3(dist*sin(t), 0.0f, -dist*cos(t)));
		rotation = vmath::rotate(t*360.0f/(2.0f * (float)M_PI), vmath::vec3(0.0f, 1.0f, 0.0f));
		view = perspective * rotation * translation;

		shader.SetUniform3fv("center", vmath::vec3((float)xpos, (float)ypos, 0.0f));
		shader.SetUniform1i("COUNT", count);
		shader.SetUniformMatrix4fv("view", view);
		
		if (statePlus == !GLFW_PRESS && (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) && dt < 0.002)
		{
			dt *= 2.0f;
		}
		if (stateMinus == !GLFW_PRESS && (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) && dt > 0.0000625f)
		{
			dt /= 2.0f;
		}
		statePlus = glfwGetKey(window, GLFW_KEY_KP_ADD);
		stateMinus = glfwGetKey(window, GLFW_KEY_KP_SUBTRACT);
		
		shader.SetUniform1f("dt", dt);

		if (!(frame % 2))
		{
			glBindTexture(GL_TEXTURE_BUFFER, tb[0]);
			glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tvb.GetID());
			glBeginTransformFeedback(GL_POINTS);
			renderer.DrawPoints(va, count, shader);
			glEndTransformFeedback();
		}
		else
		{
			glBindTexture(GL_TEXTURE_BUFFER, tb[1]);
			glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, vb.GetID());
			glBeginTransformFeedback(GL_POINTS);
			renderer.DrawPoints(tva, count, shader);
			glEndTransformFeedback();
		}
		frame++;

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
}
	glfwTerminate();
	return 0;
}