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
    constexpr uint16_t fps {60};
    constexpr float frameTime {1.0f / fps};

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
    GLuint texture0;
    glGenTextures(1, &texture0);
    glBindTexture(GL_TEXTURE_2D, texture0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int texWidth;
    int texHeight;
    int nrChannels;
    unsigned char* data = stbi_load("../../data/container2.png",
                                    &texWidth,
                                    &texHeight,
                                    &nrChannels,
                                    0);
    if (!data) {
        std::cout << "Failed to load image data.\n";
    }

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 texWidth,
                 texHeight,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    GLuint specularMap;
    glGenTextures(1, &specularMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularMap);

    data = stbi_load("../../data/container2_specular.png",
                                    &texWidth,
                                    &texHeight,
                                    &nrChannels,
                                    0);
    if (!data) {
        std::cout << "Failed to load image data.\n";
    }

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 texWidth,
                 texHeight,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    // ---
    
    // VERTICES (AND INDICES)
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
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
                          8 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          8 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          8 * sizeof(float),
                          (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


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
                          8 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(0);


    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(vertices),
                 vertices,
                 GL_STATIC_DRAW
    );
    // ---

    // TRANSFORMATIONS
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
    // ---
    glEnable(GL_DEPTH_TEST);
    lightingShader.use();

    lightingShader.setInt("material.specular", 1);
    lightingShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
    lightingShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
    lightingShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
    lightingShader.setFloat("material.shininess", 32.0f);

    lightingShader.setVec3("light.ambient",  0.2f, 0.2f, 0.2f);
    lightingShader.setVec3("light.diffuse",  0.5f, 0.5f, 0.5f); // darken diffuse light a bit
    lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

    // RENDER LOOP
    while(!glfwWindowShouldClose(window)) {
        float currentFrameTime {float(glfwGetTime())};
        globals::deltaTime = currentFrameTime - globals::lastFrameTime;
        if (globals::deltaTime < globals::frameTime) continue;
        globals::lastFrameTime = currentFrameTime;
        processInput(window);
        // rotate light source
        /*lightPos = glm::vec3(1.2f * sinf(float(glfwGetTime())),*/
        /*                     1.0f,*/
        /*                     2.0f * cosf(float(glfwGetTime())));*/


        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view;


        lightingShader.use();

        glm::vec3 lightColor(1.0f);
        /*lightColor.r = float(sin(glfwGetTime() * 2.0f));*/
        /*lightColor.g = float(sin(glfwGetTime() * 0.7f));*/
        /*lightColor.b = float(sin(glfwGetTime() * 1.3f));*/

        glm::vec3 diffuseColor = lightColor   * glm::vec3(0.5f); 
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); 
        glm::vec3 specularColor = lightColor; 

        lightingShader.setVec3("light.ambient", ambientColor);
        lightingShader.setVec3("light.diffuse", diffuseColor);
        lightingShader.setVec3("light.specular", specularColor);

        lightingShader.setVec3("light.direction", globals::myCamera.front);
        lightingShader.setFloat("light.cutoff",   glm::cos(glm::radians(12.5f)));
        lightingShader.setFloat("light.outerCutOff",   glm::cos(glm::radians(17.5f)));

        lightingShader.setFloat("light.constant",  1.0f);
        lightingShader.setFloat("light.linear",    0.09f);
        lightingShader.setFloat("light.quadratic", 0.032f);

        lightingShader.setVec3("light.position", globals::myCamera.pos);
        uint16_t i {0};
        for (glm::vec3 cubePos : cubePositions){
            glBindVertexArray(lightVao);
            model = glm::mat4(1.0f);
            model = glm::translate(model, cubePos);
            // rotate model
            /*model = glm::rotate(model, -float(glfwGetTime()), glm::vec3(0.5f, 1.0f, 0.0f));*/
            float angle {20.0f * i};
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            lightingShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            ++i;
        }

        view = globals::myCamera.getViewMatrix();
        lightingShader.setMat4("view", view);

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(globals::myCamera.zoom), 800.0f / 600.0f, 0.1f, 100.0f);
        lightingShader.setMat4("projection", projection);

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
