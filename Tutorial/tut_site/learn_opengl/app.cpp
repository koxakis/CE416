#include "app_functions.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/* select a scene theme for lighting and such */
//#define VANILLA

//#define DESERT
//#define FACTORY
#define HORROR
//#define BIOCHEMICAL_LAB

void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void set_environment_parameters();

unsigned int loadTexture(const char *path);

/* Window settings */
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

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

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main()
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

	/* Create a windowed mode window and its OpenGL context */
	GLFWwindow* window = glfwCreateWindow( SCR_WIDTH, SCR_HEIGHT, "Hello World", NULL, NULL);
	if( window == NULL )
		{
			fprintf( stderr, "Failed to open GLFW window. If you have GPU that is not 4.6 compatible. Try the 2.1 version of the tutorials.\n" );
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
		
  Shader lightingShader("basic_lighting.vs", "basic_lighting.fs");
	Shader lampShader("lamp.vs", "lamp.fs");
	/* you can name your shader files however you like */

	/* Set up vertex data (and buffer(s)) and configure vertex attributes */
	float vertices[] = 
		{
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

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
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
		};
	glm::vec3 cubePositions[] = 
	 	{
			glm::vec3( 0.0f,  0.0f,  0.0f),
			glm::vec3( 2.0f,  5.0f, -15.0f),
			glm::vec3(-1.5f, -2.2f, -2.5f),
			glm::vec3(-3.8f, -2.0f, -12.3f),
			glm::vec3( 2.4f, -0.4f, -3.5f),
			glm::vec3(-1.7f,  3.0f, -7.5f),
			glm::vec3( 1.3f, -2.0f, -2.5f),
			glm::vec3( 1.5f,  2.0f, -2.5f),
			glm::vec3( 1.5f,  0.2f, -1.5f),
			glm::vec3(-1.3f,  1.0f, -1.5f)
  	};

	/* positions of the point lights */
	glm::vec3 pointLightPositions[] = 
		{
			glm::vec3( 0.7f,  0.2f,  2.0f),
			glm::vec3( 2.3f, -3.3f, -4.0f),
			glm::vec3(-4.0f,  2.0f, -12.0f),
			glm::vec3( 0.0f,  0.0f, -3.0f)
		};
	/* Vertext Buffer Object, Vertex Array Object, Element Buffer Objects */
	/* VAO points attributes to positons in the VBO according to the stride as well as an EBO */
	/* EBO is a buffer, just like a vertex buffer object, that stores indices that OpenGL uses to decide what vertices to draw */
	unsigned int VBO, cubeVAO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	/* bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).*/
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(cubeVAO);

	/* we have to specify how OpenGL should interpret the vertex data before rendering */
	/* position attribute */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	/* normal attribute */
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);
	/* texture attribute*/
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glEnableVertexAttribArray(2);

	/* second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube) */
	unsigned int lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	/* we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes) */
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	/* note that we update the lamp's position attribute's stride to reflect the updated buffer data */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	/* load textures (we now use a utility function to keep the code more organized) */
  unsigned int diffuseMap = loadTexture(FileSystem::getPath("container2.jpg").c_str());
	unsigned int specularMap = loadTexture(FileSystem::getPath("container2_specular.png").c_str());

	/* shader configuration */
  lightingShader.use(); 
  lightingShader.setInt("material.diffuse", 0);
  lightingShader.setInt("material.specular", 1);

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
			/* NORMAL */
			#ifdef VANILLA
     		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			#endif
			/* DESERT */
			#ifdef DESERT
				glClearColor(0.75f, 0.52f, 0.3f, 1.0f);
			#endif
			/*FACTORY*/
			#ifdef FACTORY
				glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			#endif
			/*HORROR*/
			#ifdef HORROR
				glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			#endif
			/*BIOCHEMICAL LAB*/
			#ifdef BIOCHEMICAL_LAB
				glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
			#endif
			/* State using */
			/* Clear the screens colour and depth buffer */
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

			/* be sure to activate shader when setting uniforms/drawing objects */
			lightingShader.use();
			lightingShader.setVec3("viewPos", camera.Position);
			lightingShader.setFloat("material.shininess", 32.0f);

			/* light properties */
			/*
					Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index 
					the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
					by defining light types as classes and set their values in there, or by using a more efficient uniform approach
					by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
			*/
			#ifdef VANILLA
			/* directional light */
			lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
			lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
			lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
			lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
			/* point light 1 */
			lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
			lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
			lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
			lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
			lightingShader.setFloat("pointLights[0].constant", 1.0f);
			lightingShader.setFloat("pointLights[0].linear", 0.09);
			lightingShader.setFloat("pointLights[0].quadratic", 0.032);
			/* point light 2 */
			lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
			lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
			lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
			lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
			lightingShader.setFloat("pointLights[1].constant", 1.0f);
			lightingShader.setFloat("pointLights[1].linear", 0.09);
			lightingShader.setFloat("pointLights[1].quadratic", 0.032);
			/* point light 3 */
			lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
			lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
			lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
			lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
			lightingShader.setFloat("pointLights[2].constant", 1.0f);
			lightingShader.setFloat("pointLights[2].linear", 0.09);
			lightingShader.setFloat("pointLights[2].quadratic", 0.032);
			/* point light 4 */
			lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
			lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
			lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
			lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
			lightingShader.setFloat("pointLights[3].constant", 1.0f);
			lightingShader.setFloat("pointLights[3].linear", 0.09);
			lightingShader.setFloat("pointLights[3].quadratic", 0.032);
			/* spotLight */
			lightingShader.setVec3("spotLight.position", camera.Position);
			lightingShader.setVec3("spotLight.direction", camera.Front);
			lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
			lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
			lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
			lightingShader.setFloat("spotLight.constant", 1.0f);
			lightingShader.setFloat("spotLight.linear", 0.09);
			lightingShader.setFloat("spotLight.quadratic", 0.032);
			lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
			lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));   

			/* material properties */
    	lightingShader.setFloat("material.shininess", 32.0f);
			#endif 

			#ifdef DESERT
			glm::vec3 pointLightColors[] = 
				{
					glm::vec3(1.0f, 0.6f, 0.0f),
					glm::vec3(1.0f, 0.0f, 0.0f),
					glm::vec3(1.0f, 1.0, 0.0),
					glm::vec3(0.2f, 0.2f, 1.0f)
				};

			// Directional light
			lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
			lightingShader.setVec3("dirLight.ambient", 0.3f, 0.24f, 0.14f);
			lightingShader.setVec3("dirLight.diffuse", 0.7f, 0.42f, 0.26f);
			lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
			// Point light 1
			lightingShader.setVec3("pointLights[0].position", pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);		
			lightingShader.setVec3("pointLights[0].ambient", pointLightColors[0].x * 0.1,  pointLightColors[0].y * 0.1,  pointLightColors[0].z * 0.1);		
			lightingShader.setVec3("pointLights[0].diffuse", pointLightColors[0].x,  pointLightColors[0].y,  pointLightColors[0].z); 
			lightingShader.setVec3("pointLights[0].specular", pointLightColors[0].x,  pointLightColors[0].y,  pointLightColors[0].z);
			lightingShader.setFloat("pointLights[0].constant", 1.0f);
			lightingShader.setFloat("pointLights[0].linear", 0.09);
			lightingShader.setFloat("pointLights[0].quadratic", 0.032);		
			// Point light 2
			lightingShader.setVec3("pointLights[1].position", pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);		
			lightingShader.setVec3("pointLights[1].ambient", pointLightColors[1].x * 0.1,  pointLightColors[1].y * 0.1,  pointLightColors[1].z * 0.1);		
			lightingShader.setVec3("pointLights[1].diffuse", pointLightColors[1].x,  pointLightColors[1].y,  pointLightColors[1].z); 
			lightingShader.setVec3("pointLights[1].specular", pointLightColors[1].x,  pointLightColors[1].y,  pointLightColors[1].z);
			lightingShader.setFloat("pointLights[1].constant", 1.0f);
			lightingShader.setFloat("pointLights[1].linear", 0.09);
			lightingShader.setFloat("pointLights[1].quadratic", 0.032);		
			// Point light 3
			lightingShader.setVec3("pointLights[2].position", pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);		
			lightingShader.setVec3("pointLights[2].ambient", pointLightColors[2].x * 0.1,  pointLightColors[2].y * 0.1,  pointLightColors[2].z * 0.1);		
			lightingShader.setVec3("pointLights[2].diffuse", pointLightColors[2].x,  pointLightColors[2].y,  pointLightColors[2].z); 
			lightingShader.setVec3("pointLights[2].specular",pointLightColors[2].x,  pointLightColors[2].y,  pointLightColors[2].z);
			lightingShader.setFloat("pointLights[2].constant", 1.0f);
			lightingShader.setFloat("pointLights[2].linear", 0.09);
			lightingShader.setFloat("pointLights[2].quadratic", 0.032);		
			// Point light 4
			lightingShader.setVec3("pointLights[3].position", pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);		
			lightingShader.setVec3("pointLights[3].ambient", pointLightColors[3].x * 0.1,  pointLightColors[3].y * 0.1,  pointLightColors[3].z * 0.1);		
			lightingShader.setVec3("pointLights[3].diffuse", pointLightColors[3].x,  pointLightColors[3].y,  pointLightColors[3].z); 
			lightingShader.setVec3("pointLights[3].specular", pointLightColors[3].x,  pointLightColors[3].y,  pointLightColors[3].z);
			lightingShader.setFloat("pointLights[3].constant", 1.0f);
			lightingShader.setFloat("pointLights[3].linear", 0.09);
			lightingShader.setFloat("pointLights[3].quadratic", 0.032);		
			// SpotLight
			lightingShader.setVec3("spotLight.position", camera.Position.x, camera.Position.y, camera.Position.z);	
			lightingShader.setVec3("spotLight.direction", camera.Front.x, camera.Front.y, camera.Front.z);
			lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);		
			lightingShader.setVec3("spotLight.diffuse", 0.8f, 0.8f, 0.0f); 
			lightingShader.setVec3("spotLight.specular", 0.8f, 0.8f, 0.0f);
			lightingShader.setFloat("spotLight.constant", 1.0f);
			lightingShader.setFloat("spotLight.linear", 0.09);
			lightingShader.setFloat("spotLight.quadratic", 0.032);				
			lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
			lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(13.0f)));
			#endif 

			#ifdef FACTORY
			glm::vec3 pointLightColors[] = {
					glm::vec3(0.2f, 0.2f, 0.6f),
					glm::vec3(0.3f, 0.3f, 0.7f),
					glm::vec3(0.0f, 0.0f, 0.3f),
					glm::vec3(0.4f, 0.4f, 0.4f)
				};

			// Directional light
			lightingShader.setVec3("dirLight.direction",-0.2f, -1.0f, -0.3f);
			lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.1f);
			lightingShader.setVec3("dirLight.diffuse", 0.2f, 0.2f, 0.7);
			lightingShader.setVec3("dirLight.specular", 0.7f, 0.7f, 0.7f);
			// Point light 1
			lightingShader.setVec3("pointLights[0].position", pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);		
			lightingShader.setVec3("pointLights[0].ambient", pointLightColors[0].x * 0.1,  pointLightColors[0].y * 0.1,  pointLightColors[0].z * 0.1);		
			lightingShader.setVec3("pointLights[0].diffuse", pointLightColors[0].x,  pointLightColors[0].y,  pointLightColors[0].z); 
			lightingShader.setVec3("pointLights[0].specular", pointLightColors[0].x,  pointLightColors[0].y,  pointLightColors[0].z);
			lightingShader.setFloat("pointLights[0].constant", 1.0f);
			lightingShader.setFloat("pointLights[0].linear", 0.09);
			lightingShader.setFloat("pointLights[0].quadratic", 0.032);		
			// Point light 2
			lightingShader.setVec3("pointLights[1].position", pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);		
			lightingShader.setVec3("pointLights[1].ambient", pointLightColors[1].x * 0.1,  pointLightColors[1].y * 0.1,  pointLightColors[1].z * 0.1);		
			lightingShader.setVec3("pointLights[1].diffuse", pointLightColors[1].x,  pointLightColors[1].y,  pointLightColors[1].z); 
			lightingShader.setVec3("pointLights[1].specular", pointLightColors[1].x,  pointLightColors[1].y,  pointLightColors[1].z);
			lightingShader.setFloat("pointLights[1].constant", 1.0f);
			lightingShader.setFloat("pointLights[1].linear", 0.09);
			lightingShader.setFloat("pointLights[1].quadratic", 0.032);		
			// Point light 3
			lightingShader.setVec3("pointLights[2].position", pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);		
			lightingShader.setVec3("pointLights[2].ambient", pointLightColors[2].x * 0.1,  pointLightColors[2].y * 0.1,  pointLightColors[2].z * 0.1);		
			lightingShader.setVec3("pointLights[2].diffuse", pointLightColors[2].x,  pointLightColors[2].y,  pointLightColors[2].z); 
			lightingShader.setVec3("pointLights[2].specular",pointLightColors[2].x,  pointLightColors[2].y,  pointLightColors[2].z);
			lightingShader.setFloat("pointLights[2].constant", 1.0f);
			lightingShader.setFloat("pointLights[2].linear", 0.09);
			lightingShader.setFloat("pointLights[2].quadratic",0.032);		
			// Point light 4
			lightingShader.setVec3("pointLights[3].position", pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);		
			lightingShader.setVec3("pointLights[3].ambient", pointLightColors[3].x * 0.1,  pointLightColors[3].y * 0.1,  pointLightColors[3].z * 0.1);		
			lightingShader.setVec3("pointLights[3].diffuse", pointLightColors[3].x,  pointLightColors[3].y,  pointLightColors[3].z); 
			lightingShader.setVec3("pointLights[3].specular", pointLightColors[3].x,  pointLightColors[3].y,  pointLightColors[3].z);
			lightingShader.setFloat("pointLights[3].constant", 1.0f);
			lightingShader.setFloat("pointLights[3].linear", 0.09);
			lightingShader.setFloat("pointLights[3].quadratic", 0.032);		
			// SpotLight
			lightingShader.setVec3("spotLight.position", camera.Position.x, camera.Position.y, camera.Position.z);	
			lightingShader.setVec3("spotLight.direction", camera.Front.x, camera.Front.y, camera.Front.z);
			lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);		
			lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f); 
			lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
			lightingShader.setFloat("spotLight.constant", 1.0f);
			lightingShader.setFloat("spotLight.linear", 0.009);
			lightingShader.setFloat("spotLight.quadratic", 0.0032);				
			lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(10.0f)));
			lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(12.5f)));
			#endif			

			#ifdef HORROR
			glm::vec3 pointLightColors[] = {
					glm::vec3(0.1f, 0.1f, 0.1f),
					glm::vec3(0.1f, 0.1f, 0.1f),
					glm::vec3(0.1f, 0.1f, 0.1f),
					glm::vec3(0.3f, 0.1f, 0.1f)
			};

			// Directional light
			lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
			lightingShader.setVec3("dirLight.ambient", 0.0f, 0.0f, 0.0f);
			lightingShader.setVec3("dirLight.diffuse", 0.05f, 0.05f, 0.05);
			lightingShader.setVec3("dirLight.specular", 0.2f, 0.2f, 0.2f);
			// Point light 1
			lightingShader.setVec3("pointLights[0].position", pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);		
			lightingShader.setVec3("pointLights[0].ambient", pointLightColors[0].x * 0.1,  pointLightColors[0].y * 0.1,  pointLightColors[0].z * 0.1);		
			lightingShader.setVec3("pointLights[0].diffuse", pointLightColors[0].x,  pointLightColors[0].y,  pointLightColors[0].z); 
			lightingShader.setVec3("pointLights[0].specular", pointLightColors[0].x,  pointLightColors[0].y,  pointLightColors[0].z);
			lightingShader.setFloat("pointLights[0].constant", 1.0f);
			lightingShader.setFloat("pointLights[0].linear", 0.14);
			lightingShader.setFloat("pointLights[0].quadratic", 0.07);		
			// Point light 2
			lightingShader.setVec3("pointLights[1].position", pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);		
			lightingShader.setVec3("pointLights[1].ambient", pointLightColors[1].x * 0.1,  pointLightColors[1].y * 0.1,  pointLightColors[1].z * 0.1);		
			lightingShader.setVec3("pointLights[1].diffuse", pointLightColors[1].x,  pointLightColors[1].y,  pointLightColors[1].z); 
			lightingShader.setVec3("pointLights[1].specular", pointLightColors[1].x,  pointLightColors[1].y,  pointLightColors[1].z);
			lightingShader.setFloat("pointLights[1].constant", 1.0f);
			lightingShader.setFloat("pointLights[1].linear", 0.14);
			lightingShader.setFloat("pointLights[1].quadratic", 0.07);		
			// Point light 3
			lightingShader.setVec3("pointLights[2].position", pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);		
			lightingShader.setVec3("pointLights[2].ambient", pointLightColors[2].x * 0.1,  pointLightColors[2].y * 0.1,  pointLightColors[2].z * 0.1);		
			lightingShader.setVec3("pointLights[2].diffuse", pointLightColors[2].x,  pointLightColors[2].y,  pointLightColors[2].z); 
			lightingShader.setVec3("pointLights[2].specular",pointLightColors[2].x,  pointLightColors[2].y,  pointLightColors[2].z);
			lightingShader.setFloat("pointLights[2].constant", 1.0f);
			lightingShader.setFloat("pointLights[2].linear", 0.22);
			lightingShader.setFloat("pointLights[2].quadratic", 0.20);		
			// Point light 4
			lightingShader.setVec3("pointLights[3].position", pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);		
			lightingShader.setVec3("pointLights[3].ambient", pointLightColors[3].x * 0.1,  pointLightColors[3].y * 0.1,  pointLightColors[3].z * 0.1);		
			lightingShader.setVec3("pointLights[3].diffuse", pointLightColors[3].x,  pointLightColors[3].y,  pointLightColors[3].z); 
			lightingShader.setVec3("pointLights[3].specular", pointLightColors[3].x,  pointLightColors[3].y,  pointLightColors[3].z);
			lightingShader.setFloat("pointLights[3].constant", 1.0f);
			lightingShader.setFloat("pointLights[3].linear", 0.14);
			lightingShader.setFloat("pointLights[3].quadratic", 0.07);		
			// SpotLight
			lightingShader.setVec3("spotLight.position", camera.Position.x, camera.Position.y, camera.Position.z);	
			lightingShader.setVec3("spotLight.direction", camera.Front.x, camera.Front.y, camera.Front.z);
			lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);		
			lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f); 
			lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
			lightingShader.setFloat("spotLight.constant", 1.0f);
			lightingShader.setFloat("spotLight.linear", 0.09);
			lightingShader.setFloat("spotLight.quadratic", 0.032);				
			lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(7.0f)));
			lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(9.0f)));
			#endif

			#ifdef BIOCHEMICAL_LAB
			glm::vec3 pointLightColors[] = 
				{
					glm::vec3(0.4f, 0.7f, 0.1f),
					glm::vec3(0.4f, 0.7f, 0.1f),
					glm::vec3(0.4f, 0.7f, 0.1f),
					glm::vec3(0.4f, 0.7f, 0.1f)
				};

			// Directional light
			lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
			lightingShader.setVec3("dirLight.ambient", 0.5f, 0.5f, 0.5f);
			lightingShader.setVec3("dirLight.diffuse", 1.0f, 1.0f, 1.0f);
			lightingShader.setVec3("dirLight.specular", 1.0f, 1.0f, 1.0f);
			// Point light 1
			lightingShader.setVec3("pointLights[0].position", pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);		
			lightingShader.setVec3("pointLights[0].ambient", pointLightColors[0].x * 0.1,  pointLightColors[0].y * 0.1,  pointLightColors[0].z * 0.1);		
			lightingShader.setVec3("pointLights[0].diffuse", pointLightColors[0].x,  pointLightColors[0].y,  pointLightColors[0].z); 
			lightingShader.setVec3("pointLights[0].specular", pointLightColors[0].x,  pointLightColors[0].y,  pointLightColors[0].z);
			lightingShader.setFloat("pointLights[0].constant", 1.0f);
			lightingShader.setFloat("pointLights[0].linear", 0.07);
			lightingShader.setFloat("pointLights[0].quadratic", 0.017);		
			// Point light 2
			lightingShader.setVec3("pointLights[1].position", pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);		
			lightingShader.setVec3("pointLights[1].ambient", pointLightColors[1].x * 0.1,  pointLightColors[1].y * 0.1,  pointLightColors[1].z * 0.1);		
			lightingShader.setVec3("pointLights[1].diffuse", pointLightColors[1].x,  pointLightColors[1].y,  pointLightColors[1].z); 
			lightingShader.setVec3("pointLights[1].specular", pointLightColors[1].x,  pointLightColors[1].y,  pointLightColors[1].z);
			lightingShader.setFloat("pointLights[1].constant", 1.0f);
			lightingShader.setFloat("pointLights[1].linear", 0.07);
			lightingShader.setFloat("pointLights[1].quadratic", 0.017);		
			// Point light 3
			lightingShader.setVec3("pointLights[2].position", pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);		
			lightingShader.setVec3("pointLights[2].ambient", pointLightColors[2].x * 0.1,  pointLightColors[2].y * 0.1,  pointLightColors[2].z * 0.1);		
			lightingShader.setVec3("pointLights[2].diffuse", pointLightColors[2].x,  pointLightColors[2].y,  pointLightColors[2].z); 
			lightingShader.setVec3("pointLights[2].specular",pointLightColors[2].x,  pointLightColors[2].y,  pointLightColors[2].z);
			lightingShader.setFloat("pointLights[2].constant", 1.0f);
			lightingShader.setFloat("pointLights[2].linear", 0.07);
			lightingShader.setFloat("pointLights[2].quadratic", 0.017);		
			// Point light 4
			lightingShader.setVec3("pointLights[3].position", pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);		
			lightingShader.setVec3("pointLights[3].ambient", pointLightColors[3].x * 0.1,  pointLightColors[3].y * 0.1,  pointLightColors[3].z * 0.1);		
			lightingShader.setVec3("pointLights[3].diffuse", pointLightColors[3].x,  pointLightColors[3].y,  pointLightColors[3].z); 
			lightingShader.setVec3("pointLights[3].specular", pointLightColors[3].x,  pointLightColors[3].y,  pointLightColors[3].z);
			lightingShader.setFloat("pointLights[3].constant", 1.0f);
			lightingShader.setFloat("pointLights[3].linear", 0.07);
			lightingShader.setFloat("pointLights[3].quadratic", 0.017);		
			// SpotLight
			lightingShader.setVec3("spotLight.position", camera.Position.x, camera.Position.y, camera.Position.z);	
			lightingShader.setVec3("spotLight.direction", camera.Front.x, camera.Front.y, camera.Front.z);
			lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);		
			lightingShader.setVec3("spotLight.diffuse", 0.0f, 1.0f, 0.0f); 
			lightingShader.setVec3("spotLight.specular", 0.0f, 1.0f, 0.0f);
			lightingShader.setFloat("spotLight.constant", 1.0f);
			lightingShader.setFloat("spotLight.linear", 0.07);
			lightingShader.setFloat("spotLight.quadratic", 0.017);				
			lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(7.0f)));
			lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(10.0f)));
			#endif

			/* view/projection transformations */
			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			glm::mat4 view = camera.GetViewMatrix();
			lightingShader.setMat4("projection", projection);
			lightingShader.setMat4("view", view);

			/* world transformation */
			glm::mat4 model = glm::mat4(1.0f);
			lightingShader.setMat4("model", model);

			/* bind diffuse map */
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, diffuseMap);
			/* bind specular map */
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, specularMap);

			/* render containers */
			glBindVertexArray(cubeVAO);
			for (unsigned int i = 0; i < 10; i++)
				{
					/* calculate the model matrix for each object and pass it to shader before drawing */
					glm::mat4 model = glm::mat4(1.0f);
					model = glm::translate(model, cubePositions[i]);
					float angle = (20.0f * i) * glfwGetTime();
					model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
					lightingShader.setMat4("model", model);

					glDrawArrays(GL_TRIANGLES, 0, 36);
				}

			/* also draw the lamp objects */
			lampShader.use();
			lampShader.setMat4("projection", projection);
			lampShader.setMat4("view", view);

			/* many smaller cubes */
			glBindVertexArray(lightVAO);
			for (unsigned int i = 0; i < 4; i++)
				{
					model = glm::mat4(1.0f);
					model = glm::translate(model, pointLightPositions[i]);
					/* Make it a smaller cube */
					model = glm::scale(model, glm::vec3(0.2f)); 
					lampShader.setMat4("model", model);
					glDrawArrays(GL_TRIANGLES, 0, 36);
				}

			/* glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.) */
			glfwSwapBuffers(window);
			glfwPollEvents();
		}

	/* optional: de-allocate all resources once they've outlived their purpose: */
  glDeleteVertexArrays(1, &cubeVAO);
  glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &VBO);
	/* glfw: terminate, clearing all previously allocated GLFW resources. */
	glfwTerminate();

	return 0;
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

void set_environment_parameters()
{
	
}