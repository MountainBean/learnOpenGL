#include <cstdint>
#include <ostream>

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
#include <sjd/framebuffer.h>
#include <sjd/skybox.h>
#include <sjd/light.h>
#include <sjd/scene.h>
#include <sjd/meshes/cube.h>
#include <sjd/meshes/quad.h>

namespace globals {
    constexpr uint32_t windowWidth {1200};
    constexpr uint32_t windowHeight {900};
    bool actionKeyDown = false;
    bool shadowMapping = true;
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
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // INIT PLAYER
    sjd::Camera playerCamera({-1.0f, 2.0f, 5.0f});
    sjd::Player player(window, playerCamera);
    // ---

    // SHADERS
    sjd::Shader shader("../code/shaders/lighting_wShadow_map.vert.glsl",
                       "../code/shaders/blph_wShadow_map.frag.glsl");
    sjd::Shader depthShader("../code/shaders/simple_depth_shader.vert.glsl",
                            "../code/shaders/simple_depth_shader.frag.glsl");
    // ---

    // TEXTURES
    sjd::Texture cubeDiffuseMap {"../data/container2.png", true};
    sjd::Texture cubeSpecularMap {"../data/container2_specular.png", true};
    sjd::Texture floorDiffuseMap {"../data/wood.png", true};
    floorDiffuseMap.setTextureParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
    floorDiffuseMap.setTextureParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
    // ---

    // FRAMEBUFFERS
    sjd::FBTexture depthMap(globals::windowWidth, globals::windowHeight);
    // ---

    // SKYBOX
    sjd::Skybox skybox({
        "../data/skybox/right.jpg",
        "../data/skybox/left.jpg",
        "../data/skybox/top.jpg",
        "../data/skybox/bottom.jpg",
        "../data/skybox/front.jpg",
        "../data/skybox/back.jpg"
    });

    // VERTICES
    sjd::Cube cube01 {};
    cube01.setDiffuseMap(&cubeDiffuseMap);
    cube01.setSpecularMap(&cubeSpecularMap);
    sjd::Cube cube02(cube01);
    sjd::Cube cube03(cube01);

    // ARRANGE SCENE
    cube01.move(glm::vec3(0, 1.5f, 0));
    cube01.scale(glm::vec3(0.5f));
    cube02.move(glm::vec3(2, 0.5f, 1));
    cube02.scale(glm::vec3(0.5f));
    cube03.move(glm::vec3(-1, 0, 2));
    cube03.rotateX(glm::radians(30.0f));
    cube03.rotateZ(glm::radians(30.0f));
    cube03.scale(glm::vec3(0.25f));

    sjd::Quad floor({-25,-0.5,25},
                    {25,-0.5,25},
                    {25,-0.5,-25},
                    {-25,-0.5,-25});
    floor.setDiffuseMap(&floorDiffuseMap);
    sjd::Scene scene({cube01,
                      cube02,
                      cube03,
                      floor,
                     });
    scene.setSkyBox(&skybox);

    // LIGHTS
    sjd::DirLight dirLight ({-2.0f, 2.8f, -3.0});
    dirLight.enableShadowMap(&depthShader, &depthMap);
    scene.setDirLight(&dirLight);

    // RENDER LOOP
    while(!glfwWindowShouldClose(window)) {
        // TIMING
        timing.processTiming();
        if (!timing.shouldRender()) continue;

        // PLAYER INPUTS
        player.processInput(timing.getDeltaTime());
        bonus_processInput(window);
        if (globals::shadowMapping)
            dirLight.enableShadowMap(&depthShader, &depthMap);
        else dirLight.disableShadowMap();

        // MOVE OBJECTS
        cube01.reset();
        cube01.move(glm::vec3(0, 1.5f + 1.5f*sinf(0.5f*static_cast<float>(glfwGetTime())), 0));
        cube01.scale(glm::vec3(0.5f));
        cube02.reset();
        cube02.move(glm::vec3(2, 0.5f, 1));
        cube02.rotateX(static_cast<float>(glfwGetTime()));
        cube02.rotateZ(static_cast<float>(glfwGetTime()));
        cube02.scale(glm::vec3(0.5f));

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
        std::cout << "Action Key pressed..." << std::endl;
        // Do action here...
        globals::shadowMapping = !globals::shadowMapping;
    }

}
