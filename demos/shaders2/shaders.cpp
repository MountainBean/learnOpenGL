#include <cstdint>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <sjd/shader.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

const uint32_t windowWidth {800};
const uint32_t windowHeight {600};

float offset[] {0.0f, 0.0f};



int main(void) {

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create Window object
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialise GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, windowWidth, windowHeight);

    // Register the callback function so GLFW calls it
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);




    //---
    // CREATE SHADERS

    sjd::Shader ourShader("3.3.vert.glsl", "3.3.frag.glsl");

    //---
    // Create a triangle

    // create an array of normalized floating point coordinates
    float vertices[] = {
        // positions        // colors
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
        0.0f,  -0.5f, 0.0f,  0.0f, 0.0f, 1.0f
    };

    // create the buffer object in memory 
    GLuint VBO;
    glGenBuffers(1, &VBO);

    // create VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    // bind Vertex Array Object
    glBindVertexArray(VAO);

    // bind the new buffer to the GL_ARRAY_BUFFER type
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // tell OpenGL how we want to pass data to the shader program
    // define position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // define color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    // send array to the buffer currently binded to GL_ARRAY_BUFFER
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    

    // render loop
    while(!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        // rendering commands here
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // Draw the object
        ourShader.setFloat("xOffset", offset[0]);
        ourShader.setFloat("yOffset", offset[1]);
        ourShader.use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    glfwTerminate();
    return 0;
}


// callback function for when the window is resized by a user 
void framebuffer_size_callback([[maybe_unused]] GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}  

// take input from user
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        offset[1] += 0.001f;
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        offset[1] -= 0.001f;
    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        offset[0] -= 0.001f;
    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        offset[0] += 0.001f;
}
