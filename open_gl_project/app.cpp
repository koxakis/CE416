#include <iostream>
#include <cmath>
#include <signal.h>
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

#define NR_POINT_LIGHTS 11

// TODO fix error detection and halt code 
// TODO make button prompts in order to change stuff on the exhibits SOS
//				This can be done by seting the buffers to not static draw and change them in the render loop 
// TODO add which lines of code are affected in any change in the exhibit description   
// TODO fix lighting and realism  
// TODO make code more clean
// TODO update windows implementaion

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

/* Keep reading errors and display them */
/* In order to search for an error code convert to hex and look in glew.h */
static bool GLLogCall(const char* function, const char* file, int line)
{
	
	while ( GLenum currentError = glGetError() )
		{
			std::cout << "[OpenGL ERROR] (" << currentError << ") from function: " 
				<< function << " in file " << file << " at line " << line << std::endl;
			return false;
		}
	return true;
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

/* frame timing settings */
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

	/* Corridor shader */
  Shader wall_Shader("wall_shader.vs", "wall_shader.fs");
	Shader floor_Shader("floor_shader.vs", "floor_shader.fs");
	Shader celling_Shader("celling_shader.vs", "celling_shader.fs");

	Shader lampShader("lamp.vs", "lamp.fs");

	/* exhibits shader */
	Shader exhibit_triangleShader("triangle.vs", "triangle.fs");
	Shader exhibit_squareShader("triangle.vs", "triangle.fs");

	Shader exhibit_triangleColourShader("triangle.vs", "triangle.fs");
	Shader exhibit_triangleColourRotationShader("triangle.vs", "triangle.fs");

	Shader exhibit_squareTextureShader("exhibit_5_texture.vs", "exhibit_5_texture.fs");
	Shader exhibit_cubeTextureShader("exhibit_5_texture.vs", "exhibit_5_texture.fs");

	Shader exhibit_cubeMultyLightColourShader("exhibit_7_light_colour.vs", "exhibit_7_light_colour.fs");
	Shader exhibit_7_lamp("lamp.vs", "lamp.fs" );

	/* exhibit explanation shader */
	Shader exhibit_explanationShader("square.vs", "square.fs");
	Shader exhibit_explanation2Shader("square.vs", "square.fs");
	Shader exhibit_explanation3Shader("square.vs", "square.fs");
	Shader exhibit_explanation4Shader("square.vs", "square.fs");
	Shader exhibit_explanation5Shader("square.vs", "square.fs");
	Shader exhibit_explanation6Shader("square.vs", "square.fs");
	Shader exhibit_explanation7Shader("square.vs", "square.fs");
	Shader exhibit_explanation8Shader("square.vs", "square.fs");


	/* you can name your shader files however you like */

	/* Set up vertex data (and buffer(s)) and configure vertex attributes */

	float normals_cube_vertices[] = 
		{
			//position            //normals
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
				0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
				0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
				0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
				0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
				0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
				0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

				0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
				0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
				0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
				0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
				0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
				0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
				0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
				0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
				0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
				0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
				0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
				0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f		
		};

	float texture_cube_vertices[] = 
		{
			// positions          // texture coords
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
				0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
				0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
				0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
				0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
				0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
				0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
				0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
				0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
				0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
				0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
				0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
				0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
		};
    
	/* position and texture coordidnets for the wall */
	float wall_vertices[] = 
		{
			// positions          // normals           // texture coords
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

			0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f
		};

	/* position and texture coordidnets for the floor and celling */
	float floor_vertices[] =
		{
			// positions          // normals           // texture coords
			-0.5f, -0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f	
		};
	
	float celling_vertices[] =
		{
			// positions          // normals           // texture coords
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f
		};
	
	/* exhibits vertices */
	float square_triangle_vertices[] = 
		{
			// positions         // colors
			0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
			-0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom left
			0.0f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f   // top 
		};
	float square_triangle_vertices_colour[] = 
		{
			// positions         // colors
			0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
			-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
			0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top 
		};


	float square_vertices[] = 
		{
			// positions         // colors
			 0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f, // top right
			 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, // bottom right
			-0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,// bottom left
			-0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f // top left 
		};

	float square_vertices_texture[] = 
		{
			// positions          // texture coords
				0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // top right
				0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
			-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
			-0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // top left 
		};
	unsigned int square_vertices_indices[] = 
		{ // note that we start from 0!
			0, 1, 3, // first triangle
			1, 2, 3 // second triangle
		};
	/* world space positions of our exhibits */
	glm::vec3 exhibitsPositions[] = 
		{
			/* 0 exhibit 1 triangle basic*/
			glm::vec3( -1.9f,  0.0f,  1.0f),

			/* 1 exhibit 2 square basic*/
			glm::vec3( 1.9f,  0.0f,  1.0f),

			/* 2 exhibit 2 explanation */
			glm::vec3( 1.9f,  0.0f,  -1.5f),

			/* 3 exhibit 1 explanation */
			glm::vec3( -1.9f,  0.0f,  -1.5f),

			/* 4 exhibit 3 triangle tri-colour */
			glm::vec3( -1.9f,  0.0f,  -4.0f),

			/* 5 exhibit 4 triangle tri-colour rotating */
			glm::vec3( 1.4f,  0.0f,  -4.0f),	

			/* 6 exhibit 3 explanation */
			glm::vec3( 1.9f,  0.0f,  -5.5f),

			/* 7 exhibit 4 explanation */
			glm::vec3( -1.9f,  0.0f,  -5.5f),

			/* 8 exhibit 5 texture square*/
			glm::vec3( -1.9f,  0.0f,  -8.5f),

			/* 9 exhibit 6 texture cube*/
			glm::vec3( 1.4f,  0.0f,  -8.5f),

			/* 10 exhibit 6 explanation */
			glm::vec3( 1.9f,  0.0f,  -10.0f),

			/* 11 exhibit 5 explanation */
			glm::vec3( -1.9f,  0.0f,  -10.0f),

			/* 12 exhibit 7 */
			glm::vec3( -1.4f,  0.0f,  -13.0f),

			/* 13 exhibit 8 */
			glm::vec3( 1.4f,  0.0f,  -13.0f),

			/* 14 exhibit 8 explanation */
			glm::vec3( 1.9f,  0.0f,  -14.5f),

			/* 15 exhibit 7 explanation */
			glm::vec3( -1.9f,  0.0f,  -14.5f)
		};

	/* world space positions of our cubes */
	glm::vec3 cubePositions[] = 
		{
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
	/* positions of the point lights */
	glm::vec3 pointLightPositions[] = 
		{
			glm::vec3( 0.0f,  1.85f,  1.0f),
			glm::vec3( 0.0f,  1.85f, -4.0f),
			glm::vec3( 0.0f,  1.85f, -8.0f),
			glm::vec3( 0.0f,  1.85f, -12.0f),
			glm::vec3( 0.0f,  1.85f, -16.0f),
			glm::vec3( 0.0f,  1.85f, -20.0f),
			glm::vec3( 0.0f,  1.85f, -24.0f),
			glm::vec3( 0.0f,  1.85f, -28.0f),
			glm::vec3( 0.0f,  1.85f, -32.0f),
			glm::vec3( 0.0f,  1.85f, -36.0f),
			glm::vec3( 0.0f,  1.85f, -42.0f),

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	/* normal attribute */
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

	/* texture coord attribute */
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

	/* note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind */
	glBindBuffer(GL_ARRAY_BUFFER, 0); 

	/* floor buffer preparation and initialization */

	unsigned int floor_VBO, floor_VAO;
	glGenVertexArrays(1, &floor_VAO);
	glGenBuffers(1, &floor_VBO);

	/* bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).*/
	glBindVertexArray(floor_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, floor_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floor_vertices), floor_vertices, GL_STATIC_DRAW);

	/* we have to specify how OpenGL should interpret the vertex data before rendering */
	/* position attribute */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	/* normal attribute */
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

	/* texture coord attribute */
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

	/* note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind */
	glBindBuffer(GL_ARRAY_BUFFER, 0); 

	/* celling buffer preparation and initialization */

	unsigned int celling_VBO, celling_VAO;
	glGenVertexArrays(1, &celling_VAO);
	glGenBuffers(1, &celling_VBO);

	/* bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).*/
	glBindVertexArray(celling_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, celling_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(celling_vertices), celling_vertices, GL_STATIC_DRAW);

	/* we have to specify how OpenGL should interpret the vertex data before rendering */
	/* position attribute */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	/* normal attribute */
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

	/* texture coord attribute */
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

	/* note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind */
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	/* spot light sources buffer init and set */

	/* second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube) */
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	/* we only need to bind to the celling_VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes) */
	glBindBuffer(GL_ARRAY_BUFFER, celling_VBO);
	/* note that we update the lamp's position attribute's stride to reflect the updated buffer data */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	/* exhibit explenations ( and general squares that need textures ) buffer init and set */

	unsigned int exhibit_explenations_VBO, exhibit_explenations_VAO, exhibit_explenations_EBO;
	glGenVertexArrays(1, &exhibit_explenations_VAO);
	glGenBuffers(1, &exhibit_explenations_VBO);
	glGenBuffers(1, &exhibit_explenations_EBO);
	/* bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).*/
	glBindVertexArray(exhibit_explenations_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, exhibit_explenations_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square_vertices_texture), square_vertices_texture, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, exhibit_explenations_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(square_vertices_indices), square_vertices_indices, GL_STATIC_DRAW);

	/* we have to specify how OpenGL should interpret the vertex data before rendering */
	/* position attribute */
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	/* texture coord attribute */
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	/* note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind */
	glBindBuffer(GL_ARRAY_BUFFER, 0); 

	/* You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
			VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary. */
	glBindVertexArray(0); 

	/* exhibit 1 basic triangle buffer init and set */

	unsigned int exhibit_1_VBO, exhibit_1_VAO;
	glGenVertexArrays(1, &exhibit_1_VAO);
	glGenBuffers(1, &exhibit_1_VBO);
	/* bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s). */
	glBindVertexArray(exhibit_1_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, exhibit_1_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square_triangle_vertices), square_triangle_vertices, GL_STATIC_DRAW);

	/* position attribute */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	/* color attribute */
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
  
	glBindVertexArray(0);

	/* exhibit 2 basic square ( as well as any square shape ) buffer init and set */

	unsigned int exhibit_2_VBO, exhibit_2_VAO, exhibit_2_EBO;
	glGenVertexArrays(1, &exhibit_2_VAO);
	glGenBuffers(1, &exhibit_2_VBO);
	glGenBuffers(1, &exhibit_2_EBO);

	/* bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s). */
	glBindVertexArray(exhibit_2_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, exhibit_2_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square_vertices), square_vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, exhibit_2_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(square_vertices_indices), square_vertices_indices, GL_STATIC_DRAW);

	/* position attribute */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	/* color attribute */
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);	

	/* note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind */
	glBindBuffer(GL_ARRAY_BUFFER, 0); 

	glBindVertexArray(0);

	/* exhibit 3 basic triangle with colour interpolation buffer init and set */

	unsigned int exhibit_3_VBO, exhibit_3_VAO;
	glGenVertexArrays(1, &exhibit_3_VAO);
	glGenBuffers(1, &exhibit_3_VBO);
	/* bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s). */
	glBindVertexArray(exhibit_3_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, exhibit_3_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square_triangle_vertices_colour), square_triangle_vertices_colour, GL_STATIC_DRAW);

	/* position attribute */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	/* color attribute */
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
  
	glBindVertexArray(0);

	/* exhibit 6 cube with texture buffer init and set */

	unsigned int exhibit_6_VBO, exhibit_6_VAO;
	glGenVertexArrays(1, &exhibit_6_VAO);
	glGenBuffers(1, &exhibit_6_VBO);

	/* bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).*/
	glBindVertexArray(exhibit_6_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, exhibit_6_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texture_cube_vertices), texture_cube_vertices, GL_STATIC_DRAW);

	/* we have to specify how OpenGL should interpret the vertex data before rendering */
	/* position attribute */
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	/* texture coord attribute */
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	/* note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind */
	glBindBuffer(GL_ARRAY_BUFFER, 0); 

	/* You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
			VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary. */
	glBindVertexArray(0); 

	/* exhibit 7 cube with basic lighting and revolving colours buffer set and init */

	unsigned int exhibit_7_VBO, exhibit_7_VAO;
	glGenVertexArrays(1, &exhibit_7_VAO);
	glGenBuffers(1, &exhibit_7_VBO);

	/* bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).*/
	glBindBuffer(GL_ARRAY_BUFFER, exhibit_7_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals_cube_vertices), normals_cube_vertices, GL_STATIC_DRAW);

	glBindVertexArray(exhibit_7_VAO);

	/* we have to specify how OpenGL should interpret the vertex data before rendering */
	/* position attribute */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	/* normal attribute */
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

	/* You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
			VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary. */
	glBindVertexArray(0); 
	
	/* load textures (we now use a utility function to keep the code more organized) */
	/* load textures for the wall */
  unsigned int diffuseMap_wall = loadTexture(FileSystem::getPath("gray.jpg").c_str());
	unsigned int specularMap_wall = loadTexture(FileSystem::getPath("gray.jpg").c_str());

	/* load textures for the floor*/
	unsigned int diffuseMap_floor = loadTexture(FileSystem::getPath("Wooden_Wall.jpg").c_str());
	unsigned int specularMap_floor = loadTexture(FileSystem::getPath("Wooden_Wall.jpg").c_str());

	/* load textures for the celling */
	unsigned int diffuseMap_celling = loadTexture(FileSystem::getPath("celling2.jpg").c_str());
	unsigned int specularMap_celling = loadTexture(FileSystem::getPath("celling2.jpg").c_str());

	/* load textures for explenations */
	unsigned int text_texture_1 = loadTexture(FileSystem::getPath("exhibit_explenation_1.jpg").c_str());
	unsigned int text_texture_2 = loadTexture(FileSystem::getPath("exhibit_explenation_2.jpg").c_str());
	unsigned int text_texture_3 = loadTexture(FileSystem::getPath("exhibit_explenation_3.jpg").c_str());
	unsigned int text_texture_4 = loadTexture(FileSystem::getPath("exhibit_explenation_4.jpg").c_str());
	unsigned int text_texture_5 = loadTexture(FileSystem::getPath("exhibit_explenation_5.jpg").c_str());
	unsigned int text_texture_6 = loadTexture(FileSystem::getPath("exhibit_explenation_6.jpg").c_str());
	unsigned int text_texture_7 = loadTexture(FileSystem::getPath("exhibit_explenation_7.jpg").c_str());
	unsigned int text_texture_8 = loadTexture(FileSystem::getPath("exhibit_explenation_8.jpg").c_str());

	unsigned int openGL_logo = loadTexture(FileSystem::getPath("opengl.png").c_str());

	/* load textures for exhibit 5 square  with texture */
	unsigned int exhibit_5_texture_1 = loadTexture(FileSystem::getPath("container2.png").c_str());
	unsigned int exhibit_5_texture_2 = loadTexture(FileSystem::getPath("awesomeface.jpg").c_str());

	/* shader configuration */
	/* shader for the walls */
	wall_Shader.use();
	wall_Shader.setInt("material.diffuseMap_wall", 0);
	wall_Shader.setInt("material.specularMap_wall", 1);	

	/* shader for the floor */
	floor_Shader.use();
	floor_Shader.setInt("material.diffuseMap_floor",0);
	floor_Shader.setInt("material.specularMap_floor",1);

	/* shader for the floor */
	celling_Shader.use();
	celling_Shader.setInt("material.diffuseMap_celling",0);
	celling_Shader.setInt("material.specularMap_celling",1);

	exhibit_explanationShader.use();
	exhibit_explanationShader.setInt("text_texture_1", 0);
	exhibit_explanationShader.setInt("openGL_logo", 1);

	exhibit_explanation2Shader.use();
	exhibit_explanation2Shader.setInt("text_texture_2", 0);
	exhibit_explanation2Shader.setInt("openGL_logo", 1);

	exhibit_explanation3Shader.use();
	exhibit_explanation3Shader.setInt("text_texture_3", 0);
	exhibit_explanation3Shader.setInt("openGL_logo", 1);

	exhibit_explanation4Shader.use();
	exhibit_explanation4Shader.setInt("text_texture_4", 0);
	exhibit_explanation4Shader.setInt("openGL_logo", 1);
	
	exhibit_explanation5Shader.use();
	exhibit_explanation5Shader.setInt("text_texture_5", 0);
	exhibit_explanation5Shader.setInt("openGL_logo", 1);

	exhibit_explanation6Shader.use();
	exhibit_explanation6Shader.setInt("text_texture_6", 0);
	exhibit_explanation6Shader.setInt("openGL_logo", 1); 

	exhibit_explanation7Shader.use();
	exhibit_explanation7Shader.setInt("text_texture_7", 0);
	exhibit_explanation7Shader.setInt("openGL_logo", 1); 

	exhibit_explanation8Shader.use();
	exhibit_explanation8Shader.setInt("text_texture_8", 0);
	exhibit_explanation8Shader.setInt("openGL_logo", 1); 

	exhibit_squareTextureShader.use();
	exhibit_squareTextureShader.setInt("exhibit_5_texture_1", 0);
	exhibit_squareTextureShader.setInt("exhibit_5_texture_2", 1);

	exhibit_cubeTextureShader.use();
	exhibit_cubeTextureShader.setInt("exhibit_5_texture_1", 0);
	exhibit_cubeTextureShader.setInt("exhibit_5_texture_2", 1);

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

			/* exhibit 1 triangle matrix set */

			exhibit_triangleShader.use();
      glBindVertexArray(exhibit_1_VAO);

			/* pass projection matrix to shader (note that in this case it could change every frame) */
			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			exhibit_triangleShader.setMat4("projection", projection);

			/* camera/view transformation */
			/* make sure to initialize matrix to identity matrix first */
			glm::mat4 view = camera.GetViewMatrix();
			exhibit_triangleShader.setMat4("view", view);

			/* world transformation */
			glm::mat4 exhibit_a_model = glm::mat4(1.0f);
			exhibit_triangleShader.setMat4("model", exhibit_a_model);

			exhibit_a_model = glm::mat4(1.0f);

			exhibit_a_model = glm::translate(exhibit_a_model, exhibitsPositions[0]);
			/* rotate the object to on an axis xyz based on the current time */
			float angle = (float) glm::radians(90.0f);
			exhibit_a_model = glm::rotate(exhibit_a_model, angle, glm::vec3(0.0f, 1.0f, 0.0f));

			/* scale the exhibit */
			exhibit_a_model = glm::scale(exhibit_a_model, glm::vec3(1.25f)); 
			exhibit_triangleShader.setMat4("model", exhibit_a_model);
      
			glDrawArrays(GL_TRIANGLES, 0, 3);		

			/* exhibit 2 square matrix set */

			exhibit_squareShader.use();

      glBindVertexArray(exhibit_2_VAO);

			/* pass projection matrix to shader (note that in this case it could change every frame) */
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			exhibit_squareShader.setMat4("projection", projection);

			/* camera/view transformation */
			/* make sure to initialize matrix to identity matrix first */
			view = camera.GetViewMatrix();
			exhibit_squareShader.setMat4("view", view);

			/* world transformation */
			glm::mat4 exhibit_b_model = glm::mat4(1.0f);
			exhibit_squareShader.setMat4("model", exhibit_b_model);

			exhibit_b_model = glm::mat4(1.0f);

			exhibit_b_model = glm::translate(exhibit_b_model, exhibitsPositions[1]);
			/* rotate the object to on an axis xyz based on the current time */
			angle = (float) glm::radians(90.0f);
			exhibit_b_model = glm::rotate(exhibit_b_model, angle, glm::vec3(0.0f, 1.0f, 0.0f));

			/* scale the exhibit */
			exhibit_b_model = glm::scale(exhibit_b_model, glm::vec3(1.25f)); 
			exhibit_squareShader.setMat4("model", exhibit_b_model);

      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			/* exhibit 3 triangle matrix set */

			exhibit_triangleColourShader.use();
      glBindVertexArray(exhibit_3_VAO);

			/* pass projection matrix to shader (note that in this case it could change every frame) */
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			exhibit_triangleColourShader.setMat4("projection", projection);

			/* camera/view transformation */
			/* make sure to initialize matrix to identity matrix first */
			view = camera.GetViewMatrix();
			exhibit_triangleColourShader.setMat4("view", view);

			/* world transformation */
			glm::mat4 exhibit_c_model = glm::mat4(1.0f);
			exhibit_triangleColourShader.setMat4("model", exhibit_c_model);

			exhibit_c_model = glm::mat4(1.0f);

			exhibit_c_model = glm::translate(exhibit_c_model, exhibitsPositions[4]);
			/* rotate the object to on an axis xyz based on the current time */
			angle = (float) glm::radians(90.0f);
			exhibit_c_model = glm::rotate(exhibit_c_model, angle, glm::vec3(0.0f, 1.0f, 0.0f));

			/* scale the exhibit */
			exhibit_c_model = glm::scale(exhibit_c_model, glm::vec3(1.25f)); 
			exhibit_triangleColourShader.setMat4("model", exhibit_c_model);
      
			glDrawArrays(GL_TRIANGLES, 0, 3);		

			/* exhibit 4 triangle matrix set */

			exhibit_triangleColourRotationShader.use();
      glBindVertexArray(exhibit_3_VAO);

			/* pass projection matrix to shader (note that in this case it could change every frame) */
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			exhibit_triangleColourRotationShader.setMat4("projection", projection);

			/* camera/view transformation */
			/* make sure to initialize matrix to identity matrix first */
			view = camera.GetViewMatrix();
			exhibit_triangleColourRotationShader.setMat4("view", view);

			/* world transformation */
			glm::mat4 exhibit_d_model = glm::mat4(1.0f);
			exhibit_triangleColourRotationShader.setMat4("model", exhibit_d_model);

			exhibit_d_model = glm::mat4(1.0f);

			exhibit_d_model = glm::translate(exhibit_d_model, exhibitsPositions[5]);
			/* rotate the object to on an axis xyz based on the current time */
			angle = (float) (glm::radians(90.0f) * glfwGetTime());
			exhibit_d_model = glm::rotate(exhibit_d_model, angle, glm::vec3(0.0f, 1.0f, 0.0f));

			/* scale the exhibit */
			exhibit_d_model = glm::scale(exhibit_d_model, glm::vec3(1.25f)); 
			exhibit_triangleColourRotationShader.setMat4("model", exhibit_d_model);
      
			glDrawArrays(GL_TRIANGLES, 0, 3);		

			/* exhibit 5 square texture*/

			/* bind Texture */
			exhibit_squareTextureShader.use();
      glBindVertexArray(exhibit_explenations_VAO);

      glActiveTexture(GL_TEXTURE0);
    	glBindTexture(GL_TEXTURE_2D, exhibit_5_texture_1);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, exhibit_5_texture_2);

			/* pass projection matrix to shader (note that in this case it could change every frame) */
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			exhibit_squareTextureShader.setMat4("projection", projection);

			/* camera/view transformation */
			/* make sure to initialize matrix to identity matrix first */
			view = camera.GetViewMatrix();
			exhibit_squareTextureShader.setMat4("view", view);

			/* world transformation */
			glm::mat4 exhibit_e_model = glm::mat4(1.0f);
			exhibit_squareTextureShader.setMat4("model", exhibit_e_model);

			exhibit_e_model = glm::mat4(1.0f);

			exhibit_e_model = glm::translate(exhibit_e_model, exhibitsPositions[8]);
			/* rotate the object to on an axis xyz based on the current time */
			angle = (float) glm::radians(90.0f);
			exhibit_e_model = glm::rotate(exhibit_e_model, angle, glm::vec3(0.0f, 1.0f, 0.0f));

			/* scale the exhibit */
			exhibit_e_model = glm::scale(exhibit_e_model, glm::vec3(1.25f)); 
			exhibit_squareTextureShader.setMat4("model", exhibit_e_model);

      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);	

			/* exhibit 6 cube texture*/	

			exhibit_cubeTextureShader.use();
      glBindVertexArray(exhibit_6_VAO);

      glActiveTexture(GL_TEXTURE0);
    	glBindTexture(GL_TEXTURE_2D, exhibit_5_texture_1);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, exhibit_5_texture_2);

			/* pass projection matrix to shader (note that in this case it could change every frame) */
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			exhibit_squareTextureShader.setMat4("projection", projection);

			/* camera/view transformation */
			/* make sure to initialize matrix to identity matrix first */
			view = camera.GetViewMatrix();
			exhibit_squareTextureShader.setMat4("view", view);

			/* world transformation */
			glm::mat4 exhibit_f_model = glm::mat4(1.0f);
			exhibit_squareTextureShader.setMat4("model", exhibit_f_model);

			exhibit_f_model = glm::mat4(1.0f);

			exhibit_f_model = glm::translate(exhibit_f_model, exhibitsPositions[9]);
			/* rotate the object to on an axis xyz based on the current time */
			angle = (float) glm::radians(90.0f) * (float)glfwGetTime();
			exhibit_f_model = glm::rotate(exhibit_f_model, angle, glm::vec3(0.0f, 1.0f, 0.0f));

			/* scale the exhibit */
			exhibit_f_model = glm::scale(exhibit_f_model, glm::vec3(0.75f)); 
			exhibit_squareTextureShader.setMat4("model", exhibit_f_model);

			glDrawArrays(GL_TRIANGLES, 0, 36);

			/* exhibit 7 cube with basic lighting and revolving colours */

			exhibit_cubeMultyLightColourShader.use();
			glm::vec3 light_pos_exhibit_7(1.2f,  1.0f, 15.0f);

			exhibit_cubeMultyLightColourShader.setVec3("light.position", light_pos_exhibit_7);
			exhibit_cubeMultyLightColourShader.setVec3("viewPos", camera.Position);

			/* light properties */
			/* Light properties change over time basted on current time and a sin*/
			glm::vec3 lightColor;
			lightColor.x = sin(glfwGetTime() * 2.0f);
			lightColor.y = sin(glfwGetTime() * 0.7f);
			lightColor.z = sin(glfwGetTime() * 1.3f);

			/* decrease the influence */
			glm::vec3 diffuseColor = lightColor   * glm::vec3(0.5f); 
			/* low influence */
			glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); 
			
			/*  to fill the struct we still have to set the individual uniforms, but this time
					prefixed with the struct’s name: */
			exhibit_cubeMultyLightColourShader.setVec3("light.ambient", ambientColor);
			exhibit_cubeMultyLightColourShader.setVec3("light.diffuse", diffuseColor);
			exhibit_cubeMultyLightColourShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

			/* material properties */
			exhibit_cubeMultyLightColourShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
			exhibit_cubeMultyLightColourShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);

			/* specular lighting doesn't have full effect on this object's material */
			exhibit_cubeMultyLightColourShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f); 

			/* keep the shininess at 32 */
			exhibit_cubeMultyLightColourShader.setFloat("material.shininess", 32.0f);			

			/* pass projection matrix to shader (note that in this case it could change every frame) */
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			view = camera.GetViewMatrix();
			exhibit_cubeMultyLightColourShader.setMat4("projection", projection);
			exhibit_cubeMultyLightColourShader.setMat4("view", view);

			/* camera/view transformation */
			/* make sure to initialize matrix to identity matrix first */


			/* world transformation */
			glm::mat4 exhibit_g_model = glm::mat4(1.0f);
			exhibit_cubeMultyLightColourShader.setMat4("model", exhibit_g_model);

			exhibit_g_model = glm::mat4(1.0f);

			exhibit_g_model = glm::translate(exhibit_g_model, exhibitsPositions[12]);
			/* scale the exhibit */
			exhibit_g_model = glm::scale(exhibit_g_model, glm::vec3(0.75f)); 
			exhibit_cubeMultyLightColourShader.setMat4("model", exhibit_g_model); 

			glDrawArrays(GL_TRIANGLES, 0, 36);

			exhibit_7_lamp.use();
			exhibit_7_lamp.setMat4("projection", projection);
			exhibit_7_lamp.setMat4("view", view);

			glm::mat4 lamp_model2 = glm::mat4(1.0f);
			lamp_model2 = glm::translate(lamp_model2, light_pos_exhibit_7);
			/* a smaller cube */
			lamp_model2 = glm::scale(lamp_model2, glm::vec3(0.2f)); 
			exhibit_7_lamp.setMat4("model", lamp_model2);

			glBindVertexArray(exhibit_7_VAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			/* exhibit 8 cube with basic lighting and revolving colours rotating*/

			exhibit_cubeMultyLightColourShader.use();
			light_pos_exhibit_7 = glm::vec3(1.2f,  1.0f, -15.0f);

			exhibit_cubeMultyLightColourShader.setVec3("light.position", light_pos_exhibit_7);
			exhibit_cubeMultyLightColourShader.setVec3("viewPos", camera.Position);

			/* light properties */
			/* Light properties change over time basted on current time and a sin*/
			lightColor.x = sin(glfwGetTime() * 2.0f);
			lightColor.y = sin(glfwGetTime() * 0.7f);
			lightColor.z = sin(glfwGetTime() * 1.3f);

			/* decrease the influence */
			diffuseColor = lightColor   * glm::vec3(0.5f); 
			/* low influence */
			ambientColor = diffuseColor * glm::vec3(0.2f); 
			
			/*  to fill the struct we still have to set the individual uniforms, but this time
					prefixed with the struct’s name: */
			exhibit_cubeMultyLightColourShader.setVec3("light.ambient", ambientColor);
			exhibit_cubeMultyLightColourShader.setVec3("light.diffuse", diffuseColor);
			exhibit_cubeMultyLightColourShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

			/* material properties */
			exhibit_cubeMultyLightColourShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
			exhibit_cubeMultyLightColourShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);

			/* specular lighting doesn't have full effect on this object's material */
			exhibit_cubeMultyLightColourShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f); 

			/* keep the shininess at 32 */
			exhibit_cubeMultyLightColourShader.setFloat("material.shininess", 32.0f);			

			/* pass projection matrix to shader (note that in this case it could change every frame) */
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			view = camera.GetViewMatrix();
			exhibit_cubeMultyLightColourShader.setMat4("projection", projection);
			exhibit_cubeMultyLightColourShader.setMat4("view", view);

			/* camera/view transformation */
			/* make sure to initialize matrix to identity matrix first */

			/* world transformation */
			glm::mat4 exhibit_h_model = glm::mat4(1.0f);
			exhibit_cubeMultyLightColourShader.setMat4("model", exhibit_h_model);

			exhibit_h_model = glm::mat4(1.0f);

			exhibit_h_model = glm::translate(exhibit_h_model, exhibitsPositions[13]);

			/* rotate the object to on an axis xyz based on the current time */
			angle = (float) glm::radians(90.0f) * (float)glfwGetTime();
			exhibit_h_model = glm::rotate(exhibit_h_model, angle, glm::vec3(0.0f, 1.0f, 0.0f));

			/* scale the exhibit */
			exhibit_h_model = glm::scale(exhibit_h_model, glm::vec3(0.75f)); 

			exhibit_cubeMultyLightColourShader.setMat4("model", exhibit_h_model); 

			glDrawArrays(GL_TRIANGLES, 0, 36);


			/* exhibit explanation */

			/* bind Texture */
			exhibit_explanationShader.use();
      glBindVertexArray(exhibit_explenations_VAO);

      glActiveTexture(GL_TEXTURE0);
    	glBindTexture(GL_TEXTURE_2D, text_texture_1);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, openGL_logo);

			/* pass projection matrix to shader (note that in this case it could change every frame) */
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			exhibit_explanationShader.setMat4("projection", projection);

			/* camera/view transformation */
			/* make sure to initialize matrix to identity matrix first */
			view = camera.GetViewMatrix();
			exhibit_explanationShader.setMat4("view", view);

			/* world transformation */
			glm::mat4 exhibit_explanation_model = glm::mat4(1.0f);
			exhibit_explanationShader.setMat4("model", exhibit_explanation_model);

			exhibit_explanation_model = glm::mat4(1.0f);

			exhibit_explanation_model = glm::translate(exhibit_explanation_model, exhibitsPositions[3]);
			/* rotate the object to on an axis xyz based on the current time */
			angle = (float) glm::radians(90.0f);
			exhibit_explanation_model = glm::rotate(exhibit_explanation_model, angle, glm::vec3(0.0f, 1.0f, 0.0f));

			/* scale the exhibit */
			exhibit_explanation_model = glm::scale(exhibit_explanation_model, glm::vec3(1.25f)); 
			exhibit_explanationShader.setMat4("model", exhibit_explanation_model);

      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);			

			/* exhibit 2 explanation */

			/* bind Texture */
			exhibit_explanation2Shader.use();
      glBindVertexArray(exhibit_explenations_VAO);

      glActiveTexture(GL_TEXTURE0);
    	glBindTexture(GL_TEXTURE_2D, text_texture_2);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, openGL_logo);

			/* pass projection matrix to shader (note that in this case it could change every frame) */
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			exhibit_explanation2Shader.setMat4("projection", projection);

			/* camera/view transformation */
			/* make sure to initialize matrix to identity matrix first */
			view = camera.GetViewMatrix();
			exhibit_explanation2Shader.setMat4("view", view);

			/* world transformation */
			exhibit_explanation_model = glm::mat4(1.0f);
			exhibit_explanation2Shader.setMat4("model", exhibit_explanation_model);

			exhibit_explanation_model = glm::mat4(1.0f);

			exhibit_explanation_model = glm::translate(exhibit_explanation_model, exhibitsPositions[2]);
			/* rotate the object to on an axis xyz based on the current time */
			angle = (float) glm::radians(270.0f);
			exhibit_explanation_model = glm::rotate(exhibit_explanation_model, angle, glm::vec3(0.0f, 1.0f, 0.0f));

			/* scale the exhibit */
			exhibit_explanation_model = glm::scale(exhibit_explanation_model, glm::vec3(1.25f)); 
			exhibit_explanation2Shader.setMat4("model", exhibit_explanation_model);

      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);		

			/* exhibit 3 explanation */

			/* bind Texture */
			exhibit_explanation3Shader.use();
      glBindVertexArray(exhibit_explenations_VAO);

      glActiveTexture(GL_TEXTURE0);
    	glBindTexture(GL_TEXTURE_2D, text_texture_3);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, openGL_logo);

			/* pass projection matrix to shader (note that in this case it could change every frame) */
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			exhibit_explanation3Shader.setMat4("projection", projection);

			/* camera/view transformation */
			/* make sure to initialize matrix to identity matrix first */
			view = camera.GetViewMatrix();
			exhibit_explanation3Shader.setMat4("view", view);

			/* world transformation */
			exhibit_explanation_model = glm::mat4(1.0f);
			exhibit_explanation3Shader.setMat4("model", exhibit_explanation_model);

			exhibit_explanation_model = glm::mat4(1.0f);

			exhibit_explanation_model = glm::translate(exhibit_explanation_model, exhibitsPositions[7]);
			/* rotate the object to on an axis xyz based on the current time */
			angle = (float) glm::radians(90.0f);
			exhibit_explanation_model = glm::rotate(exhibit_explanation_model, angle, glm::vec3(0.0f, 1.0f, 0.0f));

			/* scale the exhibit */
			exhibit_explanation_model = glm::scale(exhibit_explanation_model, glm::vec3(1.25f)); 
			exhibit_explanation3Shader.setMat4("model", exhibit_explanation_model);

      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);	

			/* exhibit 4 explanation */

			/* bind Texture */
			exhibit_explanation4Shader.use();
      glBindVertexArray(exhibit_explenations_VAO);

      glActiveTexture(GL_TEXTURE0);
    	glBindTexture(GL_TEXTURE_2D, text_texture_4);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, openGL_logo);

			/* pass projection matrix to shader (note that in this case it could change every frame) */
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			exhibit_explanation4Shader.setMat4("projection", projection);

			/* camera/view transformation */
			/* make sure to initialize matrix to identity matrix first */
			view = camera.GetViewMatrix();
			exhibit_explanation4Shader.setMat4("view", view);

			/* world transformation */
			exhibit_explanation_model = glm::mat4(1.0f);
			exhibit_explanation4Shader.setMat4("model", exhibit_explanation_model);

			exhibit_explanation_model = glm::mat4(1.0f);

			exhibit_explanation_model = glm::translate(exhibit_explanation_model, exhibitsPositions[6]);
			/* rotate the object to on an axis xyz based on the current time */
			angle = (float) glm::radians(270.0f);
			exhibit_explanation_model = glm::rotate(exhibit_explanation_model, angle, glm::vec3(0.0f, 1.0f, 0.0f));

			/* scale the exhibit */
			exhibit_explanation_model = glm::scale(exhibit_explanation_model, glm::vec3(1.25f)); 
			exhibit_explanation4Shader.setMat4("model", exhibit_explanation_model);

      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);	

			/* exhibit 5 explanation */

			/* bind Texture */
			exhibit_explanation5Shader.use();
      glBindVertexArray(exhibit_explenations_VAO);

      glActiveTexture(GL_TEXTURE0);
    	glBindTexture(GL_TEXTURE_2D, text_texture_5);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, openGL_logo);

			/* pass projection matrix to shader (note that in this case it could change every frame) */
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			exhibit_explanation5Shader.setMat4("projection", projection);

			/* camera/view transformation */
			/* make sure to initialize matrix to identity matrix first */
			view = camera.GetViewMatrix();
			exhibit_explanation5Shader.setMat4("view", view);

			/* world transformation */
			exhibit_explanation_model = glm::mat4(1.0f);
			exhibit_explanation5Shader.setMat4("model", exhibit_explanation_model);

			exhibit_explanation_model = glm::mat4(1.0f);

			exhibit_explanation_model = glm::translate(exhibit_explanation_model, exhibitsPositions[11]);
			/* rotate the object to on an axis xyz based on the current time */
			angle = (float) glm::radians(90.0f);
			exhibit_explanation_model = glm::rotate(exhibit_explanation_model, angle, glm::vec3(0.0f, 1.0f, 0.0f));

			/* scale the exhibit */
			exhibit_explanation_model = glm::scale(exhibit_explanation_model, glm::vec3(1.25f)); 
			exhibit_explanation5Shader.setMat4("model", exhibit_explanation_model);

      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			/* exhibit 6 explanation */

			/* bind Texture */
			exhibit_explanation6Shader.use();
      glBindVertexArray(exhibit_explenations_VAO);

      glActiveTexture(GL_TEXTURE0);
    	glBindTexture(GL_TEXTURE_2D, text_texture_6);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, openGL_logo);

			/* pass projection matrix to shader (note that in this case it could change every frame) */
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			exhibit_explanation6Shader.setMat4("projection", projection);

			/* camera/view transformation */
			/* make sure to initialize matrix to identity matrix first */
			view = camera.GetViewMatrix();
			exhibit_explanation6Shader.setMat4("view", view);

			/* world transformation */
			exhibit_explanation_model = glm::mat4(1.0f);
			exhibit_explanation6Shader.setMat4("model", exhibit_explanation_model);

			exhibit_explanation_model = glm::mat4(1.0f);

			exhibit_explanation_model = glm::translate(exhibit_explanation_model, exhibitsPositions[10]);
			/* rotate the object to on an axis xyz based on the current time */
			angle = (float) glm::radians(270.0f);
			exhibit_explanation_model = glm::rotate(exhibit_explanation_model, angle, glm::vec3(0.0f, 1.0f, 0.0f));

			/* scale the exhibit */
			exhibit_explanation_model = glm::scale(exhibit_explanation_model, glm::vec3(1.25f)); 
			exhibit_explanation6Shader.setMat4("model", exhibit_explanation_model);

      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			/* exhibit 7 explanation */

			/* bind Texture */
			exhibit_explanation7Shader.use();
      glBindVertexArray(exhibit_explenations_VAO);

      glActiveTexture(GL_TEXTURE0);
    	glBindTexture(GL_TEXTURE_2D, text_texture_7);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, openGL_logo);

			/* pass projection matrix to shader (note that in this case it could change every frame) */
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			exhibit_explanation7Shader.setMat4("projection", projection);

			/* camera/view transformation */
			/* make sure to initialize matrix to identity matrix first */
			view = camera.GetViewMatrix();
			exhibit_explanation7Shader.setMat4("view", view);

			/* world transformation */
			exhibit_explanation_model = glm::mat4(1.0f);
			exhibit_explanation7Shader.setMat4("model", exhibit_explanation_model);

			exhibit_explanation_model = glm::mat4(1.0f);

			exhibit_explanation_model = glm::translate(exhibit_explanation_model, exhibitsPositions[15]);
			/* rotate the object to on an axis xyz based on the current time */
			angle = (float) glm::radians(90.0f);
			exhibit_explanation_model = glm::rotate(exhibit_explanation_model, angle, glm::vec3(0.0f, 1.0f, 0.0f));

			/* scale the exhibit */
			exhibit_explanation_model = glm::scale(exhibit_explanation_model, glm::vec3(1.25f)); 
			exhibit_explanation7Shader.setMat4("model", exhibit_explanation_model);

      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			/* exhibit 8 explanation */

			/* bind Texture */
			exhibit_explanation8Shader.use();
      glBindVertexArray(exhibit_explenations_VAO);

      glActiveTexture(GL_TEXTURE0);
    	glBindTexture(GL_TEXTURE_2D, text_texture_8);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, openGL_logo);

			/* pass projection matrix to shader (note that in this case it could change every frame) */
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			exhibit_explanation8Shader.setMat4("projection", projection);

			/* camera/view transformation */
			/* make sure to initialize matrix to identity matrix first */
			view = camera.GetViewMatrix();
			exhibit_explanation8Shader.setMat4("view", view);

			/* world transformation */
			exhibit_explanation_model = glm::mat4(1.0f);
			exhibit_explanation8Shader.setMat4("model", exhibit_explanation_model);

			exhibit_explanation_model = glm::mat4(1.0f);

			exhibit_explanation_model = glm::translate(exhibit_explanation_model, exhibitsPositions[14]);
			/* rotate the object to on an axis xyz based on the current time */
			angle = (float) glm::radians(270.0f);
			exhibit_explanation_model = glm::rotate(exhibit_explanation_model, angle, glm::vec3(0.0f, 1.0f, 0.0f));

			/* scale the exhibit */
			exhibit_explanation_model = glm::scale(exhibit_explanation_model, glm::vec3(1.25f)); 
			exhibit_explanation8Shader.setMat4("model", exhibit_explanation_model);

      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			/* be sure to activate shader when setting uniforms/drawing objects */
			wall_Shader.use();
			wall_Shader.setVec3("vewPos", camera.Position);
			wall_Shader.setFloat("material.shininess", 32.0f);

			/* light properties */
			/*
					Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index 
					the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
					by defining light types as classes and set their values in there, or by using a more efficient uniform approach
					by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
			*/		
			glm::vec3 wallPointLightColors[] = 
				{
					glm::vec3(1.0f, 1.0f, 1.0f),
					glm::vec3(1.0f, 1.0f, 1.0f),
					glm::vec3(1.0f, 1.0f, 1.0f),
					glm::vec3(1.0f, 1.0f, 1.0f),
					glm::vec3(1.0f, 1.0f, 1.0f),
					glm::vec3(1.0f, 1.0f, 1.0f),
					glm::vec3(1.0f, 1.0f, 1.0f),
					glm::vec3(1.0f, 1.0f, 1.0f),
					glm::vec3(1.0f, 1.0f, 1.0f),
					glm::vec3(1.0f, 1.0f, 1.0f),
					glm::vec3(1.0f, 1.0f, 1.0f)
				};
			/* directional light */
			wall_Shader.setVec3("dirLight.direction", 0.0f, 1.75f, 0.0f);
			wall_Shader.setVec3("dirLight.ambient", 0.1f, 0.1f, 0.1f);
			wall_Shader.setVec3("dirLight.diffuse", 0.1f, 0.1f, 0.1f);
			wall_Shader.setVec3("dirLight.specular", 0.1f, 0.1f, 0.1f);

			char attribute_buffer[50];
			/* point light attributes */
			for (int i=0; i<11; i++)
				{
					sprintf(attribute_buffer,"pointLights[%d].position", i);
					wall_Shader.setVec3(attribute_buffer, pointLightPositions[i]);
					
					sprintf(attribute_buffer,"pointLights[%d].ambient", i);
					wall_Shader.setVec3(attribute_buffer, wallPointLightColors[i].x * 0.1,  wallPointLightColors[i].y * 0.1,  wallPointLightColors[i].z * 0.1);		
										
					sprintf(attribute_buffer,"pointLights[%d].diffuse", i);
					wall_Shader.setVec3(attribute_buffer, wallPointLightColors[i].x,  wallPointLightColors[i].y,  wallPointLightColors[i].z);

					sprintf(attribute_buffer,"pointLights[%d].specular", i);
					wall_Shader.setVec3(attribute_buffer, wallPointLightColors[i].x,  wallPointLightColors[i].y,  wallPointLightColors[i].z);
										
					sprintf(attribute_buffer,"pointLights[%d].constant", i);
					wall_Shader.setFloat(attribute_buffer, 1.0f);
										
					sprintf(attribute_buffer,"pointLights[%d].linear", i);
					wall_Shader.setFloat(attribute_buffer, 0.09);
										
					sprintf(attribute_buffer,"pointLights[%d].quadratic", i);
					wall_Shader.setFloat(attribute_buffer, 0.032);									
				}

			/* pass projection matrix to shader (note that in this case it could change every frame) */
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			wall_Shader.setMat4("projection", projection);

			/* camera/view transformation */
			/* make sure to initialize matrix to identity matrix first */
			view = camera.GetViewMatrix();
			wall_Shader.setMat4("view", view);

			/* world transformation */
			glm::mat4 wall_model = glm::mat4(1.0f);
			wall_Shader.setMat4("model", wall_model);
			
			/* bind diffuse map */
      glActiveTexture(GL_TEXTURE0);
    	glBindTexture(GL_TEXTURE_2D, diffuseMap_wall);
			/* bind specular map */
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, specularMap_wall);	

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
					GLClearError();
				}

			/* activate shader */
			floor_Shader.use();
			floor_Shader.setVec3("vewPos", camera.Position);
			floor_Shader.setFloat("material.shininess", 32.0f);

			/* light properties */
			/*
					Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index 
					the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
					by defining light types as classes and set their values in there, or by using a more efficient uniform approach
					by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
			*/
			glm::vec3 floorPointLightColors[] = 
				{
					glm::vec3(0.1f, 0.1f, 0.1f),
					glm::vec3(0.1f, 0.1f, 0.1f),
					glm::vec3(0.1f, 0.1f, 0.1f),
					glm::vec3(0.1f, 0.1f, 0.1f),
					glm::vec3(0.1f, 0.1f, 0.1f),
					glm::vec3(0.1f, 0.1f, 0.1f),
					glm::vec3(0.1f, 0.1f, 0.1f),
					glm::vec3(0.1f, 0.1f, 0.1f),
					glm::vec3(0.1f, 0.1f, 0.1f),
					glm::vec3(0.1f, 0.1f, 0.1f),
					glm::vec3(0.1f, 0.1f, 0.1f)
				};
			/* directional light */
			floor_Shader.setVec3("dirLight.direction", 0.0f, 1.75f, 0.0f);
			floor_Shader.setVec3("dirLight.ambient", 0.5f, 0.5f, 0.5f);
			floor_Shader.setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
			floor_Shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);	
			/* point light attributes */
			for (int i=0; i<11; i++)
				{
					sprintf(attribute_buffer,"pointLights[%d].position", i);
					floor_Shader.setVec3(attribute_buffer, pointLightPositions[i]);
					
					sprintf(attribute_buffer,"pointLights[%d].ambient", i);
					floor_Shader.setVec3(attribute_buffer, floorPointLightColors[i].x * 0.1,  floorPointLightColors[i].y * 0.1,  floorPointLightColors[i].z * 0.1);		
										
					sprintf(attribute_buffer,"pointLights[%d].diffuse", i);
					floor_Shader.setVec3(attribute_buffer, floorPointLightColors[i].x,  floorPointLightColors[i].y,  floorPointLightColors[i].z);

					sprintf(attribute_buffer,"pointLights[%d].specular", i);
					floor_Shader.setVec3(attribute_buffer, floorPointLightColors[i].x,  floorPointLightColors[i].y,  floorPointLightColors[i].z);
										
					sprintf(attribute_buffer,"pointLights[%d].constant", i);
					floor_Shader.setFloat(attribute_buffer, 1.0f);
										
					sprintf(attribute_buffer,"pointLights[%d].linear", i);
					floor_Shader.setFloat(attribute_buffer, 0.09);
										
					sprintf(attribute_buffer,"pointLights[%d].quadratic", i);
					floor_Shader.setFloat(attribute_buffer, 0.032);									
				}

			/* pass projection matrix to shader (note that in this case it could change every frame) */
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);			
			floor_Shader.setMat4("projection", projection);

			/* camera/view transformation */
			/* make sure to initialize matrix to identity matrix first */
			view = camera.GetViewMatrix();
			floor_Shader.setMat4("view", view);

			/* world transformation */
			glm::mat4 floor_model = glm::mat4(1.0f);
			floor_Shader.setMat4("model", floor_model);

      glActiveTexture(GL_TEXTURE0);
    	glBindTexture(GL_TEXTURE_2D, diffuseMap_floor);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, specularMap_floor);

			/* render container */
			glBindVertexArray(floor_VAO);		

			/* loop in order to draw all the boxes with different attributes */
			for (unsigned int i = 0; i < 11; i++)
				{
					/* calculate the model matrix for each object and pass it to shader before drawing */
					glm::mat4 floor_model = glm::mat4(1.0f);
					/* place the cubes in various locations */
					floor_model = glm::translate(floor_model, cubePositions[i]);

					/* enlarge the cubes to make them more like a corridor*/
					floor_model = glm::scale(floor_model, glm::vec3(4.0f)); 
					/* send the model matrix to the shader */
					floor_Shader.setMat4("model", floor_model);

					GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
				}

			/* activate shader */
			celling_Shader.use();
			celling_Shader.setVec3("vewPos", camera.Position);
			celling_Shader.setFloat("material.shininess", 32.0f);

			/* light properties */
			/*
					Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index 
					the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
					by defining light types as classes and set their values in there, or by using a more efficient uniform approach
					by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
			*/	
			glm::vec3 cellingPointLightColors[] = 
				{
					glm::vec3(1.0f, 1.0f, 1.0f),
					glm::vec3(1.0f, 1.0f, 1.0f),
					glm::vec3(1.0f, 1.0f, 1.0f),
					glm::vec3(1.0f, 1.0f, 1.0f),
					glm::vec3(1.0f, 1.0f, 1.0f),
					glm::vec3(1.0f, 1.0f, 1.0f),
					glm::vec3(1.0f, 1.0f, 1.0f),
					glm::vec3(1.0f, 1.0f, 1.0f),
					glm::vec3(1.0f, 1.0f, 1.0f),
					glm::vec3(1.0f, 1.0f, 1.0f),
					glm::vec3(1.0f, 1.0f, 1.0f)
				};	
			/* directional light */
			celling_Shader.setVec3("dirLight.direction", 0.0f, 1.75f, 0.0f);
			celling_Shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
			celling_Shader.setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
			celling_Shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
			/* point light attributes */
			for (int i=0; i<11; i++)
				{
					sprintf(attribute_buffer,"pointLights[%d].position", i);
					celling_Shader.setVec3(attribute_buffer, pointLightPositions[i]);
					
					sprintf(attribute_buffer,"pointLights[%d].ambient", i);
					celling_Shader.setVec3(attribute_buffer, cellingPointLightColors[i].x * 0.1,  cellingPointLightColors[i].y * 0.1,  cellingPointLightColors[i].z * 0.1);		
										
					sprintf(attribute_buffer,"pointLights[%d].diffuse", i);
					celling_Shader.setVec3(attribute_buffer, cellingPointLightColors[i].x,  cellingPointLightColors[i].y,  cellingPointLightColors[i].z);

					sprintf(attribute_buffer,"pointLights[%d].specular", i);
					celling_Shader.setVec3(attribute_buffer, cellingPointLightColors[i].x,  cellingPointLightColors[i].y,  cellingPointLightColors[i].z);
										
					sprintf(attribute_buffer,"pointLights[%d].constant", i);
					celling_Shader.setFloat(attribute_buffer, 1.0f);
										
					sprintf(attribute_buffer,"pointLights[%d].linear", i);
					celling_Shader.setFloat(attribute_buffer, 0.09);
										
					sprintf(attribute_buffer,"pointLights[%d].quadratic", i);
					celling_Shader.setFloat(attribute_buffer, 0.032);									
				}

			/* pass projection matrix to shader (note that in this case it could change every frame) */
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);						
			celling_Shader.setMat4("projection", projection);

			/* camera/view transformation */
			/* make sure to initialize matrix to identity matrix first */
			view = camera.GetViewMatrix();
			celling_Shader.setMat4("view", view);
			
			/* world transformation */
			glm::mat4 celling_model = glm::mat4(1.0f);
			celling_Shader.setMat4("model", celling_model);

      glActiveTexture(GL_TEXTURE0);
    	glBindTexture(GL_TEXTURE_2D, diffuseMap_celling);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, specularMap_celling);

			/* render container */
			glBindVertexArray(celling_VAO);		

			/* loop in order to draw all the boxes with different attributes */
			for (unsigned int i = 0; i < 11; i++)
				{
					/* calculate the model matrix for each object and pass it to shader before drawing */
					glm::mat4 celling_model = glm::mat4(1.0f);
					/* place the cubes in various locations */
					celling_model = glm::translate(celling_model, cubePositions[i]);

					/* enlarge the cubes to make them more like a corridor*/
					celling_model = glm::scale(celling_model, glm::vec3(4.0f)); 
					/* send the model matrix to the shader */
					celling_Shader.setMat4("model", celling_model);

					glDrawArrays(GL_TRIANGLES, 0, 6);
				}


			/* also draw the lamp objects */
			lampShader.use();
			lampShader.setMat4("projection", projection);
			lampShader.setMat4("view", view);

			/* many smaller cubes */
			glBindVertexArray(lightVAO);
			for (unsigned int i = 0; i < NR_POINT_LIGHTS; i++)
				{
					glm::mat4 lamp_model = glm::mat4(1.0f);
					lamp_model = glm::translate(lamp_model, pointLightPositions[i]);
					/* Make it a smaller cube */
					lamp_model = glm::scale(lamp_model, glm::vec3(0.2f)); 
					lampShader.setMat4("model", lamp_model);
					glDrawArrays(GL_TRIANGLES, 0, 6);
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