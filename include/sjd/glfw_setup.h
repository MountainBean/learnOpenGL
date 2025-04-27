#ifndef GLFW_SETUP_H
#define GLFW_SETUP_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace sjd {
GLFWwindow* createCoreWindow(uint32_t windowWidth, uint32_t windowHeight, uint16_t msaa=1);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

inline GLFWwindow* createCoreWindow(uint32_t windowWidth, uint32_t windowHeight, uint16_t msaa){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    if (msaa > 1) {
        glfwWindowHint(GLFW_SAMPLES, 4);
    }
    GLFWwindow* window = glfwCreateWindow(windowWidth, 
                            windowHeight, 
                            "LearnOpenGL: transformations", 
                            NULL, 
                            NULL
    );
    if (!window) {
        std::cout << "Failed to create GLFW window." << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialised GLAD." << std::endl;
    }
    glViewport(0, 0, windowWidth, windowHeight);

    if (msaa > 1) {
        glEnable(GL_MULTISAMPLE);
    }
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    return window;
}

// callback function for when the window is resized by a user 
inline void framebufferSizeCallback([[maybe_unused]] GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

}
#endif
