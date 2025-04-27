#include <cstdint>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sjd/shader.h>
#include <sjd/camera.h>

GLFWwindow* createCoreWindow(uint32_t windowWidth, uint32_t windowHeight);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

namespace globals {
    constexpr uint32_t windowWidth {1200};
    constexpr uint32_t windowHeight {900};
    constexpr uint16_t fps {60};
    constexpr float frameTime {1.0f / fps};
    glm::vec3 spotLightColour(1.0f);

    float deltaTime {0.0f}; // Time between current frame and last frame
    float lastFrameTime {0.0f}; // Time of last frame
    float mouseLastX {windowWidth / 2.0f};
    float mouseLastY {windowHeight / 2.0f};
    bool firstMouse {true};
    // CAMERA
    sjd::Camera myCamera {};
    // ---
    bool spotLightOn {false};
    bool WAS_PRESSED_F {false};
}


int main(void) {
    
    // INIT WINDOW
    GLFWwindow* window {createCoreWindow(globals::windowWidth, globals::windowHeight)};
    if (!window) {return -1;}
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // ---

    // REGISTER CALLBACKS
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    // ---

    // SHADERS
    sjd::Shader shader("instancing.vert.glsl",
                       "instancing.frag.glsl");
    // ---

    // vertices
    float quadVertices[] = {
        // positions     // colors
        -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
         0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
        -0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

        -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
         0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
         0.05f,  0.05f,  0.0f, 1.0f, 1.0f
    };

    glm::vec2 translations[100];
    int index = 0;
    float offset = 0.1f;
    for(int y = -10; y < 10; y += 2)
    {
        for(int x = -10; x < 10; x += 2)
        {
            glm::vec2 translation;
            translation.x = (float)x / 10.0f + offset;
            translation.y = (float)y / 10.0f + offset;
            translations[index++] = translation;
        }
    }

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);	

    unsigned int instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100, &translations[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);	
    glVertexAttribDivisor(2, 1);

    // RENDER LOOP
    while(!glfwWindowShouldClose(window)) {
        float currentFrameTime {float(glfwGetTime())};
        globals::deltaTime = currentFrameTime - globals::lastFrameTime;
        if (globals::deltaTime < globals::frameTime) continue;
        globals::lastFrameTime = currentFrameTime;
        processInput(window);


        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        glBindVertexArray(VAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // ---

    glfwTerminate();
    return 0;
}



GLFWwindow* createCoreWindow(uint32_t windowWidth, uint32_t windowHeight){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
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
    stbi_set_flip_vertically_on_load(true);
    glEnable(GL_DEPTH_TEST);
    return window;
}

// callback function for when the window is resized by a user 
void framebufferSizeCallback([[maybe_unused]] GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}  

void mouseCallback([[maybe_unused]] GLFWwindow* window, double xpos, double ypos){

    if (globals::firstMouse) // initially set to true
    {
        globals::mouseLastX = float(xpos);
        globals::mouseLastY = float(ypos);
        globals::firstMouse = false;
    }
    float xoffset {float(xpos - globals::mouseLastX)};
    float yoffset {float(globals::mouseLastY - ypos)}; // reversed since y-coordinates range from bottom to top
    globals::mouseLastX = float(xpos);
    globals::mouseLastY = float(ypos);

    globals::myCamera.processMouseMovement(xoffset, yoffset);

}
void scrollCallback([[maybe_unused]] GLFWwindow* window,
                    [[maybe_unused]] double xoffset,
                    double yoffset) {
    globals::myCamera.processMouseScroll(float(yoffset));
}

// take input from user
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) { globals::WAS_PRESSED_F = true; }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE && globals::WAS_PRESSED_F == true) {
        globals::WAS_PRESSED_F = false;
        if (globals::spotLightOn) globals::spotLightOn = false;
        else globals::spotLightOn = true;
    };
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        globals::myCamera.processKeyboard(sjd::Camera::FORWARD,
                                          globals::deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        globals::myCamera.processKeyboard(sjd::Camera::BACKWARD,
                                          globals::deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        globals::myCamera.processKeyboard(sjd::Camera::LEFT,
                                          globals::deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        globals::myCamera.processKeyboard(sjd::Camera::RIGHT,
                                          globals::deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        globals::myCamera.processKeyboard(sjd::Camera::UP,
                                          globals::deltaTime);
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        globals::myCamera.processKeyboard(sjd::Camera::DOWN,
                                          globals::deltaTime);
}
