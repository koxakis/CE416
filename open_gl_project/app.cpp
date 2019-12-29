#include <iostream>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "camera.h"
#include "shader_s.h"
#include "filesystem.h"

/* Asserts if the return was false and lunches debug break */
#define ASSERT(x) if (!(x)) raise(SIGTRAP);

#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))


/* Keep reading and clearing OpenGL error flags */
/* A more modern approach is glDebugMessageCallback */
static void GLClearError()
{
	while (glGetError() != GL_NO_ERROR)
		{	
		}
}
/* Tell GLFW we want to call this function on every window resize by registering it */
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

/* set the callback functions to be called on every mouse event */
void processInput(GLFWwindow *window);

/* set the callback functions to be called on every mouse event */
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

unsigned int loadTexture(const char *path);

/* function that autodetects the max resolution*/
void get_resolution();

/* const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720; */

unsigned int SCR_WIDTH ;
unsigned int SCR_HEIGHT ; 

/* we need to set up a camera system */
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

/* frame timing settings*/
/* Time between current frame and last frame */
float deltaTime = 0.0f; 
/* Time of last frame */
float lastFrame = 0.0f; 

int main(int argc, char const *argv[])
{
	/* Initialize the library */
	if( !glfwInit() )
		{
			fprintf( stderr, "Failed to initialize GLFW\n" );
			getchar();
			return -1;
		}

	/* Adding extra openGL options and version */
	glfwWindowHint(GLFW_SAMPLES, 256);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	/* We don't want the old OpenGL */
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  

	/* auto retrieve and select primary monitors max resolution */
	get_resolution();

	/* Create a window in full screen and its OpenGL context */
	GLFWwindow* window = glfwCreateWindow( SCR_WIDTH, SCR_HEIGHT, "Computer Graphics Course-OpenGL Project", glfwGetPrimaryMonitor(), NULL);
	if( window == NULL )
		{
			fprintf( stderr, "Failed to open GLFW window.\n" );
			getchar();
			glfwTerminate();
			return -1;
		}
	
	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/* Tell GLFW we want to call this function on every window resize by registering it */
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	/* set the callback functions to be called on every mouse event */
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	/* tell GLFW to capture our mouse */
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);	

	/* glad: load all OpenGL function pointers */
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return -1;
		}
	/* After this point we can use all the OpenGL functionality */
	std::cout << glGetString(GL_VERSION) << std::endl;

	/* the vertex shader each input variable is also known as a vertex attribute*/
	unsigned int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, (GLint*)&nrAttributes);
	std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

	/* make sure OpenGL actually performs the depth testing we first need to tell OpenGL we want to enable depth testing */
	glEnable(GL_DEPTH_TEST);

	/* build and compile our shader program */
	/* create 2 shader objects */
	/* Vertex Shader is a part of the graphics pipeline */
	/* When we issue a draw call the Vertext shader gets
	 called first and then the Fragment shader 			*/

	/* In the triangle we have 3 verticies -> this will be called 3 times 
		to tell where in the window you want the vertex to be */ 

	/* Fragment Shader is a part of the graphics pipeline */
	/* In simple terms there are many stages in the pipeline
		before, after and between the shaders and the rasterization */

	/* Begining of draw call-> ... -> Shader(Vertex) -> ... Shader(Fragment) -> ... 
		-> rasterization -> ... -> pixels on screen */

	/* fragments aren't exactly pixels, the Fragment shader will run one time for each
		pixel that is going to be rasterized(Fill the triangle with pixels) to deside 
		witch colour(or other attribute) the pixel will be */

	/* SHADER HERE */
	/* build and compile our shader program */
  Shader wall_Shader("wall_shader.vs", "wall_shader.fs");
	Shader floor_Shader("floor_shader.vs", "wall_shader.fs");
	/* you can name your shader files however you like */

	/* Set up vertex data (and buffer(s)) and configure vertex attributes */

	/* position and texture coordidnets for the wall */
	float wall_vertices[] = 
		{
			// positions          // texture coords
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			0.5f,  0.5f, -0.5f,  0.0f, 0.0f,
			0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			0.5f, -0.5f,  0.5f,  1.0f, 1.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		};

	/* position and texture coordidnets for the floor and celling */
	float floor_celling_vertices[] =
		{
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
		};

	/* world space positions of our cubes */
	glm::vec3 cubePositions[] = {
			glm::vec3( 0.0f,  0.0f,  0.0f),
			glm::vec3( 0.0f,  0.0f, -4.0f),
			glm::vec3( 0.0f,  0.0f, -8.0f),
			glm::vec3( 0.0f,  0.0f, -12.0f),
			glm::vec3( 0.0f,  0.0f, -16.0f),
			glm::vec3( 0.0f,  0.0f, -20.0f),
			glm::vec3( 0.0f,  0.0f, -24.0f),
			glm::vec3( 0.0f,  0.0f, -28.0f),
			glm::vec3( 0.0f,  0.0f, -32.0f),
			glm::vec3( 0.0f,  0.0f, -36.0f),
			glm::vec3( 0.0f,  0.0f, -40.0f)
	};

	/* Vertext Buffer Object, Vertex Array Object, Element Buffer Objects */
	/* VAO points attributes to positons in the VBO according to the stride as well as an EBO */
	/* EBO is a buffer, just like a vertex buffer object, that stores indices that OpenGL uses to decide what vertices to draw */

	/* wall buffer preparation and initialization */

	unsigned int wall_VBO, wall_VAO;
	glGenVertexArrays(1, &wall_VAO);
	glGenBuffers(1, &wall_VBO);

	/* bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).*/
	glBindVertexArray(wall_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, wall_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wall_vertices), wall_vertices, GL_STATIC_DRAW);

	/* we have to specify how OpenGL should interpret the vertex data before rendering */
	/* position attribute */
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	/* texture coord attribute */
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	/* note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind */
	glBindBuffer(GL_ARRAY_BUFFER, 0); 

	/* floor and celling buffer preparation and initialization */

	unsigned int floor_VBO, floor_VAO;
	glGenVertexArrays(1, &floor_VAO);
	glGenBuffers(1, &floor_VBO);

	/* bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).*/
	glBindVertexArray(floor_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, floor_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floor_celling_vertices), floor_celling_vertices, GL_STATIC_DRAW);

	/* we have to specify how OpenGL should interpret the vertex data before rendering */
	/* position attribute */
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	/* note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind */
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	/* note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind */
	glBindBuffer(GL_ARRAY_BUFFER, 0); 
	
	/* load textures (we now use a utility function to keep the code more organized) */
	/* load textures for the wall */
  unsigned int wall_texture1 = loadTexture(FileSystem::getPath("brick-wall.jpg").c_str());
	unsigned int wall_texture2 = loadTexture(FileSystem::getPath("wall.jpg").c_str());

	/* load textures for the celling and floor*/
	unsigned int floor_texture1 = loadTexture(FileSystem::getPath("Wooden-Wall.jpg").c_str());
	unsigned int floor_texture2 = loadTexture(FileSystem::getPath("wall.jpg").c_str());

	/* shader configuration */
	/* shader for the walls */
	wall_Shader.use();
	wall_Shader.setInt("wall_texture1", 0);
	wall_Shader.setInt("wall_texture2", 1);	

	/* shader for the floor and celling */
	floor_Shader.use();
	floor_Shader.setInt("floor_texture1",0);
	floor_Shader.setInt("floor_texture2",1);

	/* Uncomment this call to draw in wireframe polygons. */
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	/* render loop */
	while (!glfwWindowShouldClose(window))
		{
			/* per-frame time logic */
			/* calculate the new deltaTime value */
			float currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;
			/* input */
			processInput(window);

			/* Render here */
			/* State setting function */
			/* The entire colorbuffer will be filled with the color as configured by glClearColor */   	
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

			/* State using */
			/* Clear the screens colour and depth buffer */
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

			/* bind Texture */
      glActiveTexture(GL_TEXTURE0);
    	glBindTexture(GL_TEXTURE_2D, wall_texture1);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, wall_texture2);	

			/* activate shader */
			wall_Shader.use();

			/* pass projection matrix to shader (note that in this case it could change every frame) */
			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			wall_Shader.setMat4("projection", projection);

			/* camera/view transformation */
			/* make sure to initialize matrix to identity matrix first */
			glm::mat4 view = camera.GetViewMatrix();
			wall_Shader.setMat4("view", view);

			/* render container */
			glBindVertexArray(wall_VAO);

			/* loop in order to draw all the boxes with different attributes */
			for (unsigned int i = 0; i < 11; i++)
				{
					/* calculate the model matrix for each object and pass it to shader before drawing */
					glm::mat4 wall_model = glm::mat4(1.0f);
					/* place the cubes in various locations */
					wall_model = glm::translate(wall_model, cubePositions[i]);

					/* enlarge the cubes to make them more like a corridor*/
					wall_model = glm::scale(wall_model, glm::vec3(4.0f)); 
					/* send the model matrix to the shader */
					wall_Shader.setMat4("model", wall_model);

					glDrawArrays(GL_TRIANGLES, 0, 12);
				}

      glActiveTexture(GL_TEXTURE0);
    	glBindTexture(GL_TEXTURE_2D, floor_texture1);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, floor_texture2);

			/* activate shader */
			floor_Shader.use();

			/* pass projection matrix to shader (note that in this case it could change every frame) */
			floor_Shader.setMat4("projection", projection);

			/* camera/view transformation */
			/* make sure to initialize matrix to identity matrix first */
			floor_Shader.setMat4("view", view);

			/* render container */
			glBindVertexArray(floor_VAO);		

			/* loop in order to draw all the boxes with different attributes */
			for (unsigned int i = 0; i < 11; i++)
				{
					/* calculate the model matrix for each object and pass it to shader before drawing */
					glm::mat4 floor_celling_model = glm::mat4(1.0f);
					/* place the cubes in various locations */
					floor_celling_model = glm::translate(floor_celling_model, cubePositions[i]);

					/* enlarge the cubes to make them more like a corridor*/
					floor_celling_model = glm::scale(floor_celling_model, glm::vec3(4.0f)); 
					/* send the model matrix to the shader */
					floor_Shader.setMat4("model", floor_celling_model);

					glDrawArrays(GL_TRIANGLES, 0, 12);
				}

			/* glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.) */
			glfwSwapBuffers(window);
			glfwPollEvents();
		}

	/* glfw: terminate, clearing all previously allocated GLFW resources. */
	glfwTerminate();

	return 0;
}

/* glfw: whenever the window size changed (by OS or user resize) this callback function executes */
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	/* make sure the viewport matches the new window dimensions; note that width and 
    	height will be significantly larger than specified on retina displays. */
	glViewport(0, 0, width, height);
}

/* process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly */
void processInput(GLFWwindow *window)
{
	/* glfwGetKey function that takes the window as input together with a key */
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{ 
		/* we close GLFW by setting its WindowShouldClose property to true */
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
		
}
/* glfw: whenever the mouse moves, this callback is called */
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

/* glfw: whenever the mouse scroll wheel scrolls, this callback is called */
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	
	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	stbi_set_flip_vertically_on_load(true);
	if (data)
		{
			GLenum format;
			if (nrComponents == 1)
					format = GL_RED;
			else if (nrComponents == 3)
					format = GL_RGB;
			else if (nrComponents == 4)
					format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
	else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}

	return textureID;
}

void get_resolution() {
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	SCR_WIDTH = mode->width;
	SCR_HEIGHT = mode->height;
} 