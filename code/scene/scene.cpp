#include "glm/ext/scalar_constants.hpp"
#include "glm/trigonometric.hpp"
#include <cstdint>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sjd/glfw_setup.h>
#include <sjd/timing.h>
#include <sjd/shader.h>
#include <sjd/camera.h>
#include <sjd/player.h>
#include <sjd/texture.h>
#include <sjd/light.h>

#include <sjd/meshes/cube.h>
#include <sjd/meshes/quad.h>


namespace globals {
    constexpr uint32_t windowWidth {1200};
    constexpr uint32_t windowHeight {900};
}


int main(void) {

    // INIT WINDOW
    GLFWwindow* window {sjd::createCoreWindow(globals::windowWidth, globals::windowHeight, 4)};
    sjd::Timing timing {};
    // ---

    // CONFIGURE OPENGL
    glEnable(GL_DEPTH_TEST);

    // INIT PLAYER
    sjd::Camera playerCamera({0, 2.0f, 0});
    sjd::Player player(window, playerCamera);
    // ---

    // SHADERS
    sjd::Shader cubeShader("../code/shaders/lighting.vert.glsl",
                           "../code/shaders/phong.frag.glsl");
    sjd::Shader floorShader("../code/shaders/lighting.vert.glsl",
                            "../code/shaders/phong.frag.glsl");
    // ---

    // TEXTURES
    sjd::Texture containerDiffuseMap {"../data/container2.jpg"};
    sjd::Texture containerSpecularMap {"../data/container2_specular.png"};
    sjd::Texture floorDiffuseMap {"../data/wood.png"};
    floorDiffuseMap.setTextureParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
    floorDiffuseMap.setTextureParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);

    // ---

    // VERTICES
    sjd::Cube cube(cubeShader);
    cube.setDiffuseMap(&containerDiffuseMap);
    cube.setSpecularMap(&containerSpecularMap);

    sjd::Quad floor(floorShader,
                    {-50,0,2},
                    {50,0,2},
                    {50,0,-50},
                    {-50,0,-50});
    floor.setDiffuseMap(&floorDiffuseMap);

    // LIGHTS
    sjd::DirLight dirLight {};

    sjd::PointLight pointLight(glm::vec3(2,1,-20.0f));

    // RENDER LOOP
    while(!glfwWindowShouldClose(window)) {
        // TIMING
        timing.processTiming();
        if (!timing.shouldRender()) continue;

        // PLAYER INPUTS
        player.processInput(timing.getDeltaTime());

        // CLEAR BUFFERS
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // CONFIGURE SHADERS
        glm::mat4 projection = glm::perspective(glm::radians(playerCamera.zoom), static_cast<float>(globals::windowWidth) / globals::windowHeight, 0.1f, 1000.0f);
        glm::mat4 view = playerCamera.getViewMatrix();

        // Directional Light
        /*cube.computeLight(dirLight, playerCamera.pos);*/
        /*floor.computeLight(dirLight, playerCamera.pos);*/

        // Point Light
        pointLight.moveTo({5*sinf(static_cast<float>(2*glfwGetTime())), 1.5f, -15 + 15*cosf(static_cast<float>(0.5f*glfwGetTime()))});
        cube.computeLight(pointLight, playerCamera.pos);
        floor.computeLight(pointLight, playerCamera.pos);

        // Draw objects
        pointLight.drawLightCube(projection, view);
        floor.draw(projection, view);

        cube.reset();
        cube.move(glm::vec3(0, 0.5f, -2));
        cube.draw(projection, view);
        for (int i=0; i < 10; i++) {
            cube.move(glm::vec3(0, 0, -2));
            cube.draw(projection, view);
        }

        // End Frame Processing
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // ---

    glfwTerminate();
    return 0;
}
