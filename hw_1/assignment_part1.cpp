// main.cpp
//
// This is an example of a program using glut
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_FLAT);
}
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);
	glLoadIdentity(); /*clear the matrix */
	/*viewing transformation*/
	gluLookAt(0.0,0.0,5.0,0.0,0.0,0.0,0.0,1.0,0.0);
	glBegin(GL_LINES);
	glColor3f(1.0f,0.0f,0.0f); //y is red
	glVertex3i(0,2,0);
	glVertex3i(0,-2,0);
	glColor3f(0.0f,1.0f,0.0f); //x is green
	glVertex3i(2,0,0);
	glVertex3i(-2,0,0);
	glColor3f(0.0f,0.0f,1.0f); //z is blue
	glVertex3i(0,0,2);
	glVertex3i(0,0,-2);
	glEnd();
	// The following command ensures that the drawing commands are
	// actually executed rather than being stored in a buffer
	// awaiting additional OpenGL commands
	glFlush();
}
void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);
	glMatrixMode(GL_MODELVIEW);
}
int main(int argc, char** argv)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(800, 600, "Hello World", NULL, NULL);
	if (!window)
		{
			glfwTerminate();
			return -1;
		}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/* Move glew init as per doc after we have a valid openGL context */
	if (glewInit() != GLEW_OK)
		{
			std::cout << "Error in GlewIinit()";
		}
	/* After this point we can use all the OpenGL functionality */
	std::cout << glGetString(GL_VERSION) << std::endl;

	/* Inits colour and shade model using OpenGL functions */
	init();

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			glClear(GL_COLOR_BUFFER_BIT);	
			
			/* INSERT HERE  */
			display();
			/* Swap front and back buffers */
			glfwSwapBuffers(window);
			/* Poll for and process events */
			glfwPollEvents();
		}
	glfwTerminate();
	
	return 0;
}