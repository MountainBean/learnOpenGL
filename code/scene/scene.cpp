#include "glm/ext/vector_float3.hpp"
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
#include <vector>


namespace globals {
    constexpr uint32_t windowWidth {1200};
    constexpr uint32_t windowHeight {900};
    bool actionKeyDown = false;
    bool gammaCor = true;
}

void bonus_processInput(GLFWwindow* window);

int main(void) {

    // INIT WINDOW
    GLFWwindow* window {sjd::createCoreWindow(globals::windowWidth, globals::windowHeight, 4)};
    sjd::Timing timing {60};
    // ---

    // CONFIGURE OPENGL
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_FRAMEBUFFER_SRGB);

    // INIT PLAYER
    sjd::Camera playerCamera({0, 2.0f, 0});
    sjd::Player player(window, playerCamera);
    // ---

    // SHADERS
    sjd::Shader cubeShader("../code/shaders/lighting.vert.glsl",
                           "../code/shaders/blinn_phong16.frag.glsl");
    sjd::Shader floorShader("../code/shaders/lighting.vert.glsl",
                            "../code/shaders/blinn_phong16.frag.glsl");
    // ---

    // TEXTURES
    sjd::Texture containerDiffuseMap {"../data/container2.jpg", true};
    sjd::Texture containerSpecularMap {"../data/container2_specular.png"};
    sjd::Texture floorDiffuseMap {"../data/wood.png", true};
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
    glm::vec3 pointLightColour({1.0f, 1.0f, 1.0f});
    std::vector<sjd::PointLight> pointLights;
    pointLights.push_back(sjd::PointLight({4, 1.5, -2}, pointLightColour));
    pointLights.push_back(sjd::PointLight({4, 1.5, -6}, pointLightColour));
    pointLights.push_back(sjd::PointLight({4, 1.5, -10}, pointLightColour));
    pointLights.push_back(sjd::PointLight({4, 1.5, -14}, pointLightColour));
    pointLights.push_back(sjd::PointLight({4, 1.5, -18}, pointLightColour));
    pointLights.push_back(sjd::PointLight({4, 1.5, -22}, pointLightColour));
    pointLights.push_back(sjd::PointLight({4, 1.5, -26}, pointLightColour));
    pointLights.push_back(sjd::PointLight({4, 1.5, -30}, pointLightColour));
    pointLights.push_back(sjd::PointLight({4, 1.5, -34}, pointLightColour));

    // RENDER LOOP
    while(!glfwWindowShouldClose(window)) {
        // TIMING
        timing.processTiming();
        if (!timing.shouldRender()) continue;

        // PLAYER INPUTS
        player.processInput(timing.getDeltaTime());
        bonus_processInput(window);

        // CLEAR BUFFERS
        glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // CONFIGURE SHADERS
        glm::mat4 projection = glm::perspective(glm::radians(playerCamera.zoom), static_cast<float>(globals::windowWidth) / globals::windowHeight, 0.1f, 1000.0f);
        glm::mat4 view = playerCamera.getViewMatrix();

        // Directional Light
        /*cube.computeLight(dirLight, playerCamera.pos);*/
        /*floor.computeLight(dirLight, playerCamera.pos);*/

        // Point Lights
        cube.computeLight(pointLights, playerCamera.pos);
        floor.computeLight(pointLights, playerCamera.pos);

        // Draw objects
        for (sjd::PointLight currPointLight : pointLights) {
            currPointLight.drawLightCube(projection, view);
        }
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



void bonus_processInput(GLFWwindow* window) {

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        globals::actionKeyDown = true;
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE && globals::actionKeyDown) {
        globals::actionKeyDown = false;
        // Do action here...
        // ...
    }

}
