#include <cstdint>
#include <vector>

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
#include <sjd/scene.h>

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
    sjd::Camera playerCamera({0, 2.0f, 3});
    sjd::Player player(window, playerCamera);
    // ---

    // SHADERS
    sjd::Shader shader("../code/shaders/lighting.vert.glsl",
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
    sjd::Cube cube {};
    cube.setDiffuseMap(&containerDiffuseMap);
    cube.setSpecularMap(&containerSpecularMap);

    sjd::Quad floor({-25,-0.5,25},
                    {25,-0.5,25},
                    {25,-0.5,-25},
                    {-25,-0.5,-25});
    floor.setDiffuseMap(&floorDiffuseMap);
    sjd::Scene scene({cube, floor});

    // LIGHTS
    sjd::DirLight dirLight {glm::normalize(glm::vec3{1, 2, 1})};
    scene.setDirLight(&dirLight);

    glm::vec3 pointLightColour({1.0f, 1.0f, 1.0f});
    sjd::PointLight pointLight01({4, 0.5, -2}, pointLightColour, true);
    sjd::PointLight pointLight02({2, 0.5, 2}, pointLightColour, true);
    scene.setPointLights({
        pointLight01,
        pointLight02});
    // RENDER LOOP
    while(!glfwWindowShouldClose(window)) {
        // TIMING
        timing.processTiming();
        if (!timing.shouldRender()) continue;

        // PLAYER INPUTS
        player.processInput(timing.getDeltaTime());
        bonus_processInput(window);

        // ARRANGE SCENE
        cube.reset();
        cube.move(glm::vec3(0, 0, -3));

        // CLEAR BUFFERS
        glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // CONFIGURE SHADERS
        glm::mat4 projection = glm::perspective(glm::radians(playerCamera.zoom), static_cast<float>(globals::windowWidth) / globals::windowHeight, 0.1f, 1000.0f);
        glm::mat4 view = playerCamera.getViewMatrix();
        scene.m_projection = projection;
        scene.m_view = view;
        scene.m_viewPos = playerCamera.pos;

        // Draw objects
        scene.draw(shader);

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
