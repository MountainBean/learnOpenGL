#ifndef PLAYER_H
#define PLAYER_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <sjd/camera.h>
#include <sjd/mouse_input.h>

namespace sjd {
class Player {
public:

    Player(GLFWwindow* window, sjd::Camera& playerCamera)
    :   m_window {window}
    ,   m_playerCamera {playerCamera}
    ,   m_playerMouse {window}
    {
    }

    // take input from user
    void processInput(float deltaTime)
    {
        glm::vec2 mouse_movement {m_playerMouse.getoffsets()};
        m_playerCamera.processMouseMovement(mouse_movement.x,
                                            mouse_movement.y);
        m_playerCamera.processMouseScroll(m_playerMouse.getScroll());
        if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(m_window, true);
        if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
            m_playerCamera.processKeyboard(sjd::Camera::FORWARD,
                                              deltaTime);
        if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
            m_playerCamera.processKeyboard(sjd::Camera::BACKWARD,
                                              deltaTime);
        if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
            m_playerCamera.processKeyboard(sjd::Camera::LEFT,
                                              deltaTime);
        if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
            m_playerCamera.processKeyboard(sjd::Camera::RIGHT,
                                              deltaTime);
        if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS)
            m_playerCamera.processKeyboard(sjd::Camera::UP,
                                              deltaTime);
        if (glfwGetKey(m_window, GLFW_KEY_C) == GLFW_PRESS)
            m_playerCamera.processKeyboard(sjd::Camera::DOWN,
                                              deltaTime);
    }

private:
    GLFWwindow* m_window;
    Camera& m_playerCamera;
    Mouse m_playerMouse;
};
}
#endif
