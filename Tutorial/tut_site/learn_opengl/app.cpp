#include "app_functions.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
/* Window settings */
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

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
	glfwWindowHint(GLFW_SAMPLES, 128);
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
  Shader ourShader("shader.vs", "shader.fs");
	/* you can name your shader files however you like */

	/* Set up vertex data (and buffer(s)) and configure vertex attributes */
	float vertices[] = 
		{
			// positions          // colors           // texture coords
        // positions          // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // top left 
		};
	unsigned int indices[] = 
		{
			0, 1, 3, // first triangle
			1, 2, 3  // second triangle
		};

	/* Vertext Buffer Object, Vertex Array Object, Element Buffer Objects */
	/* VAO points attributes to positons in the VBO according to the stride as well as an EBO */
	/* EBO is a buffer, just like a vertex buffer object, that stores indices that OpenGL uses to decide what vertices to draw */
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	/* bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).*/
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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
  data = stbi_load(FileSystem::getPath("matrix.jpg").c_str(), &width, &height, &nrChannels, 0);
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

	/* Uncomment this call to draw in wireframe polygons. */
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	/* render loop */
	while (!glfwWindowShouldClose(window))
		{
			/* input */
			processInput(window);

			/* Render here */
			/* State setting function */
			/* The entire colorbuffer will be filled with the color as configured by glClearColor */
			glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
			/* State using */
			/* Clear the screens colour buffer */
      glClear(GL_COLOR_BUFFER_BIT);

			/* bind Texture */
      glActiveTexture(GL_TEXTURE0);
    	glBindTexture(GL_TEXTURE_2D, texture1);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, texture2);

			/* activate shader */
			ourShader.use();
		
			/* create transformations */
			/* make sure to initialize matrix to identity matrix first */
			glm::mat4 model         = glm::mat4(1.0f); 
			glm::mat4 view          = glm::mat4(1.0f);
			glm::mat4 projection    = glm::mat4(1.0f);
			model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
			projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

			/* retrieve the matrix uniform locations */
			unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
			unsigned int viewLoc  = glGetUniformLocation(ourShader.ID, "view");

			/* pass them to the shaders (3 different ways) */
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
			/* note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once. */
			ourShader.setMat4("projection", projection);
			
			/* render container */
			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			/* glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.) */
			glfwSwapBuffers(window);
			glfwPollEvents();
		}

	/* optional: de-allocate all resources once they've outlived their purpose: */
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	/* glfw: terminate, clearing all previously allocated GLFW resources. */
	glfwTerminate();

	return 0;
}