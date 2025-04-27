#ifndef MOUSE_INPUT_H
#define MOUSE_INPUT_H
#include "glm/ext/vector_float2.hpp"
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace sjd {
static float mouseX {};
static float mouseY {};
static glm::vec2 offsets {};
static float scrollY {};
static bool firstMouse {true};

static void mouseCallback([[maybe_unused]] GLFWwindow* window, double xpos, double ypos);
static void scrollCallback([[maybe_unused]] GLFWwindow* window, [[maybe_unused]] double xoffset, double yoffset);

class Mouse {
public:
    Mouse(GLFWwindow* window)
    : m_window {window}
    {
        int windowW {};
        int windowH {};
        glfwGetWindowSize(m_window, &windowW, &windowH);
        mouseX = windowW / 2.0f;
        mouseY = windowH / 2.0f;
        glfwSetCursorPosCallback(m_window, mouseCallback);
        glfwSetScrollCallback(m_window, scrollCallback);
    }

    glm::vec2 getoffsets() {
        glm::vec2 outOffsets(offsets);
        offsets = {0,0};
        return outOffsets;
    }

    float getScroll() {
        float outScrollY {scrollY};
        scrollY = 0;
        return outScrollY;
    }

private:
    GLFWwindow* m_window;

};

static void mouseCallback([[maybe_unused]] GLFWwindow* window, double xpos, double ypos){
    if (firstMouse) // initially set to true
    {
        mouseX = float(xpos);
        mouseY = float(ypos);
        firstMouse = false;
    }
    offsets.x = static_cast<float>(xpos) - mouseX;
    offsets.y = mouseY - static_cast<float>(ypos); // reversed since y-coordinates range from bottom to top
    mouseX = float(xpos);
    mouseY = float(ypos);
}

static void scrollCallback([[maybe_unused]] GLFWwindow* window,
                    [[maybe_unused]] double xoffset,
                    double yoffset) {
    scrollY = static_cast<float>(yoffset);
}
}
#endif
