#include <algorithm>
#include <cstdint>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <sjd/shader.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

const uint32_t windowWidth {800};
const uint32_t windowHeight {600};

float offset[] {0.0f, 0.0f};
float smileyFade {0.0f};



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

    // ---

    // Create texture gl object
    GLuint texture1;
    glGenTextures(1, &texture1); 
    glActiveTexture(GL_TEXTURE0); // Unnecesary. GL_TEXURE0 is default
    glBindTexture(GL_TEXTURE_2D, texture1);

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Load texture data
    int width;
    int height;
    int nrChannels;
    unsigned char* data = stbi_load("container.jpg", 
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
                 GL_RGB,            // data format
                 GL_UNSIGNED_BYTE,  // datatype
                 data               // image data
    );
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    GLuint texture2;
    glGenTextures(1, &texture2);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    stbi_set_flip_vertically_on_load(true); // flip the y in image data
    data = stbi_load("awesomeface.png",
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

    // create the buffer object in memory 
    GLuint VBO;
    glGenBuffers(1, &VBO);
    GLuint EBO;
    glGenBuffers(1, &EBO);

    // create VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    // bind Vertex Array Object
    glBindVertexArray(VAO);

    // bind the new buffer to the GL_ARRAY_BUFFER type
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    // tell OpenGL how we want to pass data to the shader program
    // define position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // define color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
    // define texture location attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6* sizeof(float)));
    glEnableVertexAttribArray(2);

    // send array to the buffer currently binded to GL_ARRAY_BUFFER
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    ourShader.use();
    ourShader.setInt("texture2", 1);

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
        ourShader.setFloat("smileyFade", smileyFade);
        ourShader.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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
        smileyFade = std::clamp(smileyFade + 0.001f, 0.0f, 1.0f);
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        smileyFade = std::clamp(smileyFade - 0.001f, 0.0f, 1.0f);
}
