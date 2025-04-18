#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/trigonometric.hpp"
#include <algorithm>
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

GLFWwindow* createCoreWindow(uint32_t windowWidth, uint32_t windowHeight);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

float offset[] {0.0f, 0.0f};

int main(void) {
    const uint32_t windowWidth {800};
    const uint32_t windowHeight {600};
    
    // INIT WINDOW
    GLFWwindow* window {createCoreWindow(windowWidth, windowHeight)};
    if (!window) {return -1;}
    // ---

    // REGISTER CALLBACKS
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    // ---

    // SHADERS
    sjd::Shader myShader {"3.3.vert.glsl", "3.3.frag.glsl"};
    // ---

    // TEXTURES
    GLuint texture0;
    glGenTextures(1, &texture0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    int width;
    int height;
    int nrChannels;
    unsigned char* data = stbi_load("../data/container.jpg",
                                    &width,
                                    &height,
                                    &nrChannels,
                                    0
    );
    if (!data) {
        std::cout << "Faiiled to load image data.\n";
    }

    glTexImage2D(GL_TEXTURE_2D,     // texture target
                 0,                 // mipmap level
                 GL_RGB,            // texture format
                 width,
                 height, 
                 0,                 // 0 (legacy parameter)
                 GL_RGB,            // data format
                 GL_UNSIGNED_BYTE,  // datatype
                 data               // image data
    );
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    GLuint texture1;
    glGenTextures(1, &texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    stbi_set_flip_vertically_on_load(true); // flip the y in image data
    data = stbi_load("../data/ahhprofile.png",
                     &width,
                     &height,
                     &nrChannels,
                     0
    );
    if (!data) {
        std::cout << "Failed to load image data.\n";
    }

    glTexImage2D(GL_TEXTURE_2D,     // texture target
                 0,                 // mipmap level
                 GL_RGB,            // texture format
                 width,
                 height, 
                 0,                 // 0 (legacy parameter)
                 GL_RGBA,            // data format
                 GL_UNSIGNED_BYTE,  // datatype
                 data               // image data
    );
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    myShader.use();
    myShader.setInt("texture1", 1);
    // ---
    
    // VERTICES (AND INDICES)
    float vertices[] = {
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
    // ---

    // INITILISE BUFFERS AND ATTRIBUTE ARRAYS
    GLuint VBO;
    glGenBuffers(1, &VBO);
    GLuint VAO;
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          5 * sizeof(float),
                          (void*)0
    );
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          5 * sizeof(float),
                          (void*)(3 * sizeof(float))
    );
    glEnableVertexAttribArray(1);

    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(vertices),
                 vertices,
                 GL_STATIC_DRAW
    );
    // ---

    // TRANSFORMATIONS
    std::array<glm::vec3, 10> cubePositions = {
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

    // RENDER LOOP
    while(!glfwWindowShouldClose(window)) {

        processInput(window);

        glClearColor(0.3f, 0.2f, 0.2f, 0.1f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        myShader.use();

        glBindVertexArray(VAO);
        int i {0};
        for (glm::vec3 trans : cubePositions) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, trans);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            model = glm::rotate(model, float(glfwGetTime()), glm::vec3(0.5f, 1.0f, 0.0f));
            myShader.setMat4("model", model);
            int modelLoc = glGetUniformLocation(myShader.m_id, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
            ++i;
        }
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection;

        view = glm::translate(view, glm::vec3(0.0f + offset[0], 0.0f + offset[1], -3.0f));
        projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        myShader.setMat4("view", view);
        myShader.setMat4("projection", projection);

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

// take input from user
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        offset[0] = std::clamp(offset[0] + 0.0001f, -3.0f, 3.0f);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        offset[0] = std::clamp(offset[0] - 0.0001f, -3.0f, 3.0f);
    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        offset[1] = std::clamp(offset[1] + 0.0001f, -3.0f, 3.0f);
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        offset[1] = std::clamp(offset[1] - 0.0001f, -3.0f, 3.0f);
}
