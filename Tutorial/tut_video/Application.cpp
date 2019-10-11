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
	/* By defult projection matrix in openGL is between -1.0 and 1.0 in aevery dimention in every axis */

	/* Change the colour to yello */
	glColor3f(1, 1, 0);

	/* Upper triangle */
	glVertex2f(0.0f, 1.0f);
	glVertex2f(-0.5f, 0.0f);
	glVertex2f(0.5f, 0.0f);

	/* Lower left triangle */
	glVertex2f(-0.5f, 0.0f);
	glVertex2f(-1.0f, -1.0f);
	glVertex2f(0.0f, -1.0f);

	/* Lower right triangle */
	glVertex2f(0.5f, 0.0f);
	glVertex2f(1.0f, -1.0f);
	glVertex2f(0.0f, -1.0f);

	/* End legacy openGL routine*/
	glEnd();
}

/* complile the shader from the given source */
static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(GL_VERTEX_SHADER);

	const char* src = source.c_str();

	glShaderSource(id, 1, &src, nullptr);

	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);

	if (result == GL_FALSE)
	{
		int length;

		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

		char* message = (char*)alloca(length* sizeof(char));

		glGetShaderInfoLog(id, length, &length, message);

		std::cout << "Faild to complile" <<
			(type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader" << std::endl;

		std::cout << message << std::endl;

		glDeleteShader(id);

		return 0;

	}

	return id;

}

/* Create and link the shader ( both vertext and fragment */
static unsigned int Create_Shader(const std::string& vertexShader, const std::string& fragmentShader)
{
	/* Create a program and return an identifier to it */
	unsigned int program = glCreateProgram();

	/* create 2 shader objects */
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);

	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glLinkProgram(program);

	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
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
	glBindBuffer( GL_ARRAY_BUFFER, buffer);

	/* Provide the buffer with data */
	/* You can provide the data later if need be*/
	glBufferData( GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

	/* Define a verctex atribiute */
	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	/* Enable of the generic vertex atribioute */
	//glEnableVertexAttribArray(0);

	// stopped at writing a shader in openGL 24:32
	std::string vertexShader =
		"#version 330 core\n"
		"\n"
		"void main()\n"
		"{\n"
		""
		"}\n";
	//unsigned int shader = Create_Shader();

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			glClear(GL_COLOR_BUFFER_BIT);
			
			/* Simple legacy-Immidiade mode OpenGL */
			legacy_triangles();

			/* Issue a draw call for the above buffer */
			/* glDrawArrays when you don't have index buffer */
			/* glDrawElements when you have index buffer */
			glDrawArrays(GL_TRIANGLES, 0, 3);

			/* A Shader is a program that runs on the GPU gets the data from buffer ( GPU VRAM ) */
	
			/* Swap front and back buffers */
			glfwSwapBuffers(window);
	
			/* Poll for and process events */
			glfwPollEvents();
		}

	glfwTerminate();
	return 0;
}
