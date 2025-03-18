#include "glm/ext/matrix_transform.hpp"
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





int main(void) {
    const uint32_t windowWidth {800};
    const uint32_t windowHeight {600};
    
    // INIT
    GLFWwindow* window {createCoreWindow(windowWidth, windowHeight)};
    if (!window) {return -1;}
    // ---

    // CALLBACKS
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
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };
    // ---

    // INITILISE BUFFERS AND ATTRIBUTE ARRAYS
    GLuint VBO;
    glGenBuffers(1, &VBO);
    GLuint EBO;
    glGenBuffers(1, &EBO);
    GLuint VAO;
    glGenVertexArrays(1, &VAO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          8 * sizeof(float),
                          (void*)0
    );
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          8 * sizeof(float),
                          (void*)(3 * sizeof(float))
    );
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          8 * sizeof(float),
                          (void*)(6 * sizeof(float))
    );
    glEnableVertexAttribArray(2);

    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(vertices),
                 vertices,
                 GL_STATIC_DRAW
    );
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(indices),
                 indices,
                 GL_STATIC_DRAW
    );
    // ---

    // TRANSFORMATIONS
    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
    trans = glm::rotate(trans, float(glfwGetTime()), glm::vec3(0.0f, 0.0f, 1.0f));

    GLuint transformLoc = glGetUniformLocation(myShader.m_id, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
    // ---

    // RENDER LOOP
    while(!glfwWindowShouldClose(window)) {

        processInput(window);

        glClearColor(0.3f, 0.2f, 0.2f, 0.1f);
        glClear(GL_COLOR_BUFFER_BIT);

        trans = glm::mat4(1.0f);
        trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
        trans = glm::rotate(trans, float(glfwGetTime()), glm::vec3(0.0f, 0.0f, 1.0f));
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
        
        myShader.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        trans = glm::mat4(1.0f);
        trans = glm::translate(trans, glm::vec3(-0.5f, 0.5f, 0.0f));
        trans = glm::scale(trans, glm::vec3(0.5f * sin(glfwGetTime()) + 0.5f,
                                            0.5f * sin(glfwGetTime()) + 0.5f,
                                            1.0f
                                            ));
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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
}
