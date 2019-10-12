#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
/* position data for the buffer */
float positions[6] =
{
	-0.5f, -0.5f,
	 0.0f, 0.5f,
	 0.5f, -0.5f
};

void legacy_triangles()
{
	/* Begin legacy openGL triangle routine */
	/* This can be used without the Glew Init */
	glBegin(GL_TRIANGLES);

	/* Specify 3 vertices for triangles in 2D */
	/* By defult projection matrix in openGL is between -1.0 and 1.0 in every dimensions in every axis */

	/* Change the colour to yellow */
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

/* Compile the shader from the given source */
static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	/* Specifies the type of shader to be created of type */
	unsigned int id = glCreateShader(type);

	/* This returns pointer to the begining of the data */
	/* Source needs to exists, if source goes out of scope 
			this will return garbage												*/
	const char* src = source.c_str();

	/* Point to source character data */
	glShaderSource(id, 1, &src, nullptr);

	/* Compile the shader given by the id */
	glCompileShader(id);

	int result;
	/* Return the result */
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);

	/* Error handling */
	if (result == GL_FALSE)
		{
			int length;

			/* Returns a parameter from a shader object in order to identify the problem shader */
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

			/* Allocate memory in Stack for error message ( not advisable ) */
			char* message = (char*)alloca(length* sizeof(char));

			/* Specifies the shader object whose information log is to be queried */
			glGetShaderInfoLog(id, length, &length, message);

			/* Print the shader failed massage with the accompanying message */
			std::cout << "Failed to compile" <<
				(type == type ? "vertex" : "fragment") << "shader" << std::endl;

			std::cout << message << std::endl;

			/* Delete the problem shader */
			glDeleteShader(id);

			return 0;

		}

	return id;
}

/* Create, compile and link the shader ( both vertext and fragment )*/
/* These strings are mend to be the actual source code of the shaders */
/* Can be read from files or exists as strings or genereded on the fly */  
/* Return a unique identifier for that program as per buffer in order to bind it */
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
	/* Create a program and return an identifier to it */
	unsigned int program = glCreateProgram();

	/* create 2 shader objects */
	/* Vertex Shader is a part of the graphics pipeline */
	/* When we issue a draw call the Vertext shader gets
	 called first and then the Fragment shader 			*/
	/* In the triangle we have 3 verticies -> this will be called 3 times 
		to tell where in the window you want the vertex to be */ 
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);

	/* Fragment Shader is a part of the graphics pipeline */
	/* In simple terms there are many stages in the pipeline
		before, after and between the shaders and the rasterization */
	/* Begining of draw call-> ... -> Shader(Vertex) -> ... Shader(Fragment) -> ... 
		-> rasterization -> ... -> pixels on screen */
	/* fragments aren't exactly pixels, the fragmed shader will run one time for each
		pixel that is going to be rasterized(Fill the triangle with pixels) to deside 
		witch colour(or other attribute) the pixel will be */
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	/* After the compilation we need to attach these shaders to our program */
	glAttachShader(program, vs);
	glAttachShader(program, fs);

	/* Link the program */
	glLinkProgram(program);

	/* Validate program */
	glValidateProgram(program);

	/* Delete unneeded shaders due to the linking to the program we can delete the intermediate obj files */
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
	window = glfwCreateWindow(800, 600, "Hello World", NULL, NULL);
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

	/* Define the data outside the loop, updates can happen in loop */
	/* Define OpenGL buffers */
	
	unsigned int buffer;
	/* This creates a buffer and returns the ID in that form */
	glGenBuffers(1, &buffer);

	/* Select == bind this buffer to use it */
	/* No size definition here */
	/* This is what is going to be draw */
	glBindBuffer( GL_ARRAY_BUFFER, buffer);

	/* Provide the buffer with data */
	/* You can provide the data later if need be */
	glBufferData( GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

	/* Define a vertex attribute */
	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	/* Enable of the generic vertex attribute */
	glEnableVertexAttribArray(0);

	/* String code for vertex shader */
	std::string vertexShader =
		"#version 460 core\n"
		"\n"
		"layout(location = 0) in vec4 position;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	gl_Position = position;\n"
		"}\n";

	/* String code for vertex shader */
	std::string fragmentShader =
		"#version 460 core\n"
		"\n"
		"layout(location = 0) out vec4 colour;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	colour = vec4(1.0, 1.0, 0.0, 1.0);\n"
		"}\n";
	
	/* Create shader using the above code */
	unsigned int shader = CreateShader(vertexShader, fragmentShader);

	/* Bind shader */
	glUseProgram(shader);

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

			/* A Shader is a program that runs on the GPU gets the data from buffer ( GPU VRAM ) */
	
			/* Swap front and back buffers */
			glfwSwapBuffers(window);
	
			/* Poll for and process events */
			glfwPollEvents();
		}

	glDeleteProgram(shader);

	glfwTerminate();
	return 0;
}
