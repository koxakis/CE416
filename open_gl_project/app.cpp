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
	Shader floor_Shader("floor_shader.vs", "floor_shader.fs");
	Shader celling_Shader("celling_shader.vs", "celling_shader.fs");

	Shader lampShader("lamp.vs", "lamp.fs");
	/* you can name your shader files however you like */

	/* Set up vertex data (and buffer(s)) and configure vertex attributes */

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
			-0.5f, -0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f	
		};
	
	float celling_vertices[] =
		{
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f
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
			glm::vec3( 0.0f,  1.75f,  0.0f),
			glm::vec3( 0.0f,  1.75f, -4.0f),
			glm::vec3( 0.0f,  1.75f, -8.0f),
			glm::vec3( 0.0f,  1.75f, -12.0f),
			glm::vec3( 0.0f,  1.75f, -16.0f),
			glm::vec3( 0.0f,  1.75f, -20.0f),
			glm::vec3( 0.0f,  1.75f, -24.0f),
			glm::vec3( 0.0f,  1.75f, -28.0f),
			glm::vec3( 0.0f,  1.75f, -32.0f),
			glm::vec3( 0.0f,  1.75f, -36.0f),
			glm::vec3( 0.0f,  1.75f, -40.0f),

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
	
	/* second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube) */
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	/* we only need to bind to the celling_VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes) */
	glBindBuffer(GL_ARRAY_BUFFER, celling_VBO);
	/* note that we update the lamp's position attribute's stride to reflect the updated buffer data */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	/* load textures (we now use a utility function to keep the code more organized) */
	/* load textures for the wall */
  unsigned int diffuseMap_wall = loadTexture(FileSystem::getPath("celling2.jpg").c_str());
	unsigned int specularMap_wall = loadTexture(FileSystem::getPath("celling2.jpg").c_str());

	/* load textures for the floor*/
	unsigned int diffuseMap_floor = loadTexture(FileSystem::getPath("Wooden-Wall.jpg").c_str());
	unsigned int specularMap_floor = loadTexture(FileSystem::getPath("Wooden-Wall.jpg").c_str());

	/* load textures for the celling */
	unsigned int diffuseMap_celling = loadTexture(FileSystem::getPath("celling.jpg").c_str());
	unsigned int specularMap_celling = loadTexture(FileSystem::getPath("celling2_diffuse.jpg").c_str());

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
					glm::vec3(0.5f, 0.5f, 0.5f),
					glm::vec3(0.5f, 0.5f, 0.5f),
					glm::vec3(0.5f, 0.5f, 0.5f),
					glm::vec3(0.5f, 0.5f, 0.5f),
					glm::vec3(0.5f, 0.5f, 0.5f),
					glm::vec3(0.5f, 0.5f, 0.5f),
					glm::vec3(0.5f, 0.5f, 0.5f),
					glm::vec3(0.5f, 0.5f, 0.5f),
					glm::vec3(0.5f, 0.5f, 0.5f),
					glm::vec3(0.5f, 0.5f, 0.5f),
					glm::vec3(0.5f, 0.5f, 0.5f)
				};
			/* directional light */
			wall_Shader.setVec3("dirLight.direction",0.0f, 0.0f, 0.0f);
			wall_Shader.setVec3("dirLight.ambient", 0.5f, 0.5f, 0.5f);
			wall_Shader.setVec3("dirLight.diffuse", 0.5f, 0.5f, 0.5f);
			wall_Shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

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
			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			wall_Shader.setMat4("projection", projection);

			/* camera/view transformation */
			/* make sure to initialize matrix to identity matrix first */
			glm::mat4 view = camera.GetViewMatrix();
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
			floor_Shader.setVec3("dirLight.direction", 0.5f, 0.5f, 0.5f);
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
					glm::vec3(0.75f, 0.75f, 0.75f),
					glm::vec3(0.75f, 0.75f, 0.75f),
					glm::vec3(0.75f, 0.75f, 0.75f),
					glm::vec3(0.75f, 0.75f, 0.75f),
					glm::vec3(0.75f, 0.75f, 0.75f),
					glm::vec3(0.75f, 0.75f, 0.75f),
					glm::vec3(0.75f, 0.75f, 0.75f),
					glm::vec3(0.75f, 0.75f, 0.75f),
					glm::vec3(0.75f, 0.75f, 0.75f),
					glm::vec3(0.75f, 0.75f, 0.75f),
					glm::vec3(0.75f, 0.75f, 0.75f)
				};	
			/* directional light */
			celling_Shader.setVec3("dirLight.direction",0.0f, 0.0f, 0.0f);
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
			for (unsigned int i = 0; i < 11; i++)
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