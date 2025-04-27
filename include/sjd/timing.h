#ifndef TIMING_H
#define TIMING_H
#include <GLFW/glfw3.h>

namespace sjd {
class Timing {
public:

    Timing(uint16_t fpsLimit=0) 
    : m_deltaTime {0.0f}
    , m_lastFrameTime {0.0f}
    , m_fps {fpsLimit}
    {
        (m_fps > 0) ? m_frameTime = (1.0f / m_fps) : m_frameTime = 0;
    }
    // Get the uptime of the current window and update the deltatime
    void processTiming() {
        float currentFrameTime {float(glfwGetTime())};
        m_deltaTime = currentFrameTime - m_lastFrameTime;
        if (m_fps == 0) {
            m_lastFrameTime = currentFrameTime;
        }
    }

    float getDeltaTime () {
        return m_deltaTime;
    }

    // return true if frame should be rendered
    bool shouldRender() {
        if (m_deltaTime < m_frameTime) {
            return false;
        }
        else {
            return true;
            m_lastFrameTime = static_cast<float>(glfwGetTime());
        }

}

private:
    float m_deltaTime; // Time between current frame and last frame
    float m_lastFrameTime; // Time of last frame
    uint16_t m_fps;
    float m_frameTime;
};


}
#endif
