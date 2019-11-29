
#include "camera.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cmath>

#include "shader_s.h"
#include "filesystem.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);