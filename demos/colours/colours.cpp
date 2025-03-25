#include "glm/ext/vector_float3.hpp"
#include <array>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION    // configure stb headers before include

#include <cstdint>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>

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
    float deltaTime {0.0f}; // Time between current frame and last frame
    float lastFrameTime {0.0f}; // Time of last frame
    float mouseLastX {400.0f};
    float mouseLastY {300.0f};
    bool firstMouse {true};
    // CAMERA
    sjd::Camera myCamera {};
    // ---
}


int main(void) {
    const uint32_t windowWidth {800};
    const uint32_t windowHeight {600};
    
    // INIT WINDOW
    GLFWwindow* window {createCoreWindow(windowWidth, windowHeight)};
    if (!window) {return -1;}
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // ---

    // REGISTER CALLBACKS
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    // ---

    // SHADERS
    sjd::Shader lightingShader {"lighting.vert.glsl", "lighting.frag.glsl"};
    sjd::Shader licubeShader {"licube.vert.glsl", "licube.frag.glsl"};
    // ---

    // TEXTURES
    //
    // ---
    
    // VERTICES (AND INDICES)
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

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
    // ---

    // INITILISE BUFFERS AND ATTRIBUTE ARRAYS
    GLuint vbo;
    glGenBuffers(1, &vbo);

    GLuint lightVao;
    glGenVertexArrays(1, &lightVao);

    glBindVertexArray(lightVao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          6 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          6 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    GLuint licubeVao;
    glGenVertexArrays(1, &licubeVao);
    glBindVertexArray(licubeVao);
    // we only need to bind to the VBO, the container's VBO's data already contains the data.
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // set the vertex attribute 
    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          6 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(0);


    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(vertices),
                 vertices,
                 GL_STATIC_DRAW
    );
    // ---

    // TRANSFORMATIONS
    //
    // ---
    glEnable(GL_DEPTH_TEST);
    lightingShader.use();
    lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
    lightingShader.setVec3("lightColor",  1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

    // RENDER LOOP
    while(!glfwWindowShouldClose(window)) {
        float currentFrameTime {float(glfwGetTime())};
        globals::deltaTime = currentFrameTime - globals::lastFrameTime;
        globals::lastFrameTime = currentFrameTime;
        processInput(window);
        // rotate light source
        /*lightPos = glm::vec3(1.2f * sinf(glfwGetTime()),*/
        /*                     1.0f,*/
        /*                     2.0f * cosf(glfwGetTime()));*/


        glClearColor(0.05f, 0.06f, 0.1f, 0.1f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view;


        lightingShader.use();
        lightingShader.setVec3("inLightPos", lightPos);

        glBindVertexArray(lightVao);
        model = glm::rotate(model, float(glfwGetTime()), glm::vec3(0.5f, 1.0f, 0.0f));
        lightingShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        view = globals::myCamera.getViewMatrix();
        glm::mat4 projection;
        projection = glm::perspective(glm::radians(globals::myCamera.zoom), 800.0f / 600.0f, 0.1f, 100.0f);
        lightingShader.setMat4("view", view);
        lightingShader.setMat4("projection", projection);

        // light source

        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));

        licubeShader.use();
        // set the model, view and projection matrix uniforms
        licubeShader.setMat4("model", model);
        licubeShader.setMat4("view", view);
        licubeShader.setMat4("projection", projection);
        // draw the light cube object
        glBindVertexArray(licubeVao);
        glDrawArrays(GL_TRIANGLES, 0, 36);

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
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
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
