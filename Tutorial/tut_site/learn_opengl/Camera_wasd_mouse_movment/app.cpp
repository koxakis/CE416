#include "app_functions.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
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

void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

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
		
  Shader ourShader("shader.vs", "shader.fs");
	/* you can name your shader files however you like */

	/* Set up vertex data (and buffer(s)) and configure vertex attributes */
	float vertices[] = 
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
    
	/* world space positions of our cubes */
	glm::vec3 cubePositions[] = {
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
	/* Vertext Buffer Object, Vertex Array Object, Element Buffer Objects */
	/* VAO points attributes to positons in the VBO according to the stride as well as an EBO */
	/* EBO is a buffer, just like a vertex buffer object, that stores indices that OpenGL uses to decide what vertices to draw */
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	/* bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).*/
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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

	/* load and create a texture */
	unsigned int texture1, texture2;
	/* Texture 1 */
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1); 
	/* all upcoming GL_TEXTURE_2D operations now have effect on this texture object */
	/* set the texture wrapping parameters */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	/* set texture wrapping to GL_REPEAT (default wrapping method) */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	/* set texture filtering parameters */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	/* load image, create texture and generate mipmaps */
	int width, height, nrChannels;
	/* The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path. */
	unsigned char *data = stbi_load(FileSystem::getPath("container.jpg").c_str(), &width, &height, &nrChannels, 0);
	if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
	else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
	stbi_image_free(data);

	/* Texture 2 */
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2); 
	/* all upcoming GL_TEXTURE_2D operations now have effect on this texture object */
	/* set the texture wrapping parameters */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	/* set texture wrapping to GL_REPEAT (default wrapping method) */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	/* set texture filtering parameters */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	stbi_set_flip_vertically_on_load(true);
	/* The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path. */
  data = stbi_load(FileSystem::getPath("awesomeface.jpg").c_str(), &width, &height, &nrChannels, 0);
	if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
	else
		{
			std::cout << "Failed to load texture" << std::endl;
		}
	stbi_image_free(data);

	/* tell opengl for each sampler to which texture unit it belongs to (only has to be done once) */
	ourShader.use(); 
	ourShader.setInt("texture1", 0);
	ourShader.setInt("texture2", 1);

	/* make sure OpenGL actually performs the depth testing we first need to tell OpenGL we want to enable depth testing */
	glEnable(GL_DEPTH_TEST);
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
			glClearColor(1.0f, 0.5f, 0.31f, 1.0f);
			/* State using */
			/* Clear the screens colour and depth buffer */
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

			/* bind Texture */
      glActiveTexture(GL_TEXTURE0);
    	glBindTexture(GL_TEXTURE_2D, texture1);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, texture2);

			/* activate shader */
			ourShader.use();

			/* pass projection matrix to shader (note that in this case it could change every frame) */
			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			ourShader.setMat4("projection", projection);

			/* camera/view transformation */
			/* make sure to initialize matrix to identity matrix first */
			glm::mat4 view = camera.GetViewMatrix();
			ourShader.setMat4("view", view);

			/* render container */
			glBindVertexArray(VAO);

			/* loop in order to draw all the boxes with different attributes */
			for (unsigned int i = 0; i < 10; i++)
				{
					/* calculate the model matrix for each object and pass it to shader before drawing */
					glm::mat4 model = glm::mat4(1.0f);
					/* place the cubes in various locations */
					model = glm::translate(model, cubePositions[i]);

					/* Simulate and explosion */
					//glm::vec3 position = cubePositions[i] * (glm::vec3)glfwGetTime() ;
					//printf("%lf %lf %lf %lf %lf %lf %lf %lf %lf \n", position.p, position.r, position.s, position.t, position.x, position.y, position.z, position.b, position.g);
					//model = glm::translate(model,position );

					/* change the angle of rotation according to the time and itterator */
					float angle = (20.0f * i) * (float)glfwGetTime();
					/* let’s transform our plane a bit by rotating it on the any-axis with a variable speed*/
					model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
					/* send the model matrix to the shader */
					ourShader.setMat4("model", model);

					glDrawArrays(GL_TRIANGLES, 0, 36);
				}

			/* glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.) */
			glfwSwapBuffers(window);
			glfwPollEvents();
		}

	/* optional: de-allocate all resources once they've outlived their purpose: */
	glDeleteVertexArrays(1, &VAO);
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