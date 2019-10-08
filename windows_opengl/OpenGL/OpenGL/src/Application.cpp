#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
/* BUILD a function for legacy triangle */

/* position data for the buffer */
float positions[6] =
{
	-0.5f, -0.5f,
	 0.0f, 0.5f,
	 0.5f, -0.5f
};

void legacy_triangles()
{
	/* Begin legacy openGL triagle routine */
	/* This can be used without the Glew Init */
	glBegin(GL_TRIANGLES);

	/* Specify 3 veritcies for triagles in 2D */
	/* By defult projection matrix in openGL is between -1.0 and 1.0 in aevery dimention in every axis*/
	glVertex2f(-0.5f, -0.5f);
	glVertex2f(0.0f, 0.5f);
	glVertex2f(0.5f, -0.5f);

	/* End legacy openGL routine*/
	glEnd();
}

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
		{
			glfwTerminate();
			return -1;
		}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/* Move glew init as per doc after we have a valid openGL context*/
	if (glewInit() != GLEW_OK)
		{
			std::cout << "Error in GlewIinit()";
		}
	/* After this point we can use all the OpenGL functionality */
	std::cout << glGetString(GL_VERSION) << std::endl;

	/* Define the data outside the loop, updades can happen in loop*/
	/* Define OpenGL buffers */
	
	unsigned int buffer;
	/* This creates a buffer and returns the ID in that form*/
	glGenBuffers(1, &buffer);

	/* Select == bind this buffer to use it */
	/* No size definition here */
	/* This is what is going to be draw */
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	/* Provide the buffer with data */
	/* You can provide the data later if need be*/
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			glClear(GL_COLOR_BUFFER_BIT);
			
			/* Simple legacy-Immidiade mode OpenGL */
			//legacy_triangles();

			/* Issue a draw call for the above buffer */
			/* glDrawArrays when you don't have index buffer */
			/* glDrawElements when you have index buffer */
			glDrawArrays(GL_TRIANGLES, 0, 3);

			/* Define a verctex atribiute */

			/* A Shader is a program that runs on the GPU gets the data from buffer ( GPU VRAM ) */
	
			/* Swap front and back buffers */
			glfwSwapBuffers(window);
	
			/* Poll for and process events */
			glfwPollEvents();
		}

	glfwTerminate();
	return 0;
}