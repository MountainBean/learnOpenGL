#include "glm/ext/matrix_float4x4.hpp"
#include <cstdint>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <memory>
#include <sjd/shader.h>
#include <sjd/camera.h>
#include <logl/model.h>

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
    sjd::Camera myCamera(glm::vec3(0.0f, 0.0f, 110.0f));
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

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // SHADERS
    sjd::Shader shader("3.3.vert.glsl",
                       "3.3.frag.glsl");
    sjd::Shader instanceShader("3.3.instancing.vert.glsl",
                               "3.3.frag.glsl");

    // models
    Model rock("model_asteroid/rock.obj");
    Model planet("model_planet/planet.obj");

    // model matrices
    unsigned int amount = 100000;
    auto modelMatrices = std::make_unique<glm::mat4[]>(amount);
    srand(static_cast<unsigned int>(glfwGetTime())); // initialize random seed	
    float radius = 150.0f;
    float offset = 25.0f;
    for(unsigned int i = 0; i < amount; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        // 1. translation: displace along circle with 'radius' in range [-offset, offset]
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // keep height of field smaller compared to width of x and z
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        // 2. scale: scale between 0.05 and 0.25f
        float scale = (rand() % 20) / 100.0f + 0.05f;
        model = glm::scale(model, glm::vec3(scale));

        // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
        float rotAngle = static_cast<float>(rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        // 4. now add to list of matrices
        modelMatrices[i] = model;
    }

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
      
    for(unsigned int i = 0; i < rock.meshes.size(); i++)
    {
        glBindVertexArray(rock.meshes[i].VAO);
        // vertex attributes
        glEnableVertexAttribArray(3); 
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
        glEnableVertexAttribArray(4); 
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5); 
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(6); 
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }  
    glBindBuffer(GL_ARRAY_BUFFER, 0);	

    // RENDER LOOP
    while(!glfwWindowShouldClose(window)) {
        float currentFrameTime {float(glfwGetTime())};
        globals::deltaTime = currentFrameTime - globals::lastFrameTime;
        if (globals::deltaTime < globals::frameTime) continue;
        globals::lastFrameTime = currentFrameTime;
        processInput(window);


        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // configure transformation matrices
        glm::mat4 projection = glm::perspective(glm::radians(globals::myCamera.zoom), static_cast<float>(globals::windowWidth) / static_cast<float>(globals::windowHeight), 0.1f, 1000.0f);
        glm::mat4 view = globals::myCamera.getViewMatrix();;
        instanceShader.use();
        instanceShader.setMat4("projection", projection);
        instanceShader.setMat4("view", view);
        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        // draw planet
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
        model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
        shader.setMat4("model", model);
        planet.Draw(shader);

        // draw meteorites
        instanceShader.use();
        rock.DrawInstanced(instanceShader, amount);

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
