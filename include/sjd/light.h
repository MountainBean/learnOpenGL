#ifndef LIGHT_H
#define LIGHT_H
#include "glm/geometric.hpp"
#include <array>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <sjd/shader.h>
namespace sjd {

static const std::array<float, 108> lightCubeVertices {
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,

     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
};

class Light {
protected:
    Light(glm::vec3 position={0.0f, -1.0f, 0.0f},
             glm::vec3 colour={1.0f, 1.0f, 1.0f},
             bool gamma=false)
    :   m_position {position}
    ,   m_colour    {colour}
    ,   m_ambient   {0.08f}
    ,   m_diffuse   {0.6f}
    ,   m_specular  {0.8f}
    ,   m_gamma     {gamma}
    {
    }

public:
    virtual void computeLight(sjd::Shader& shader, [[maybe_unused]] unsigned int id=0) const = 0;

protected:
    glm::vec3 m_position;
    glm::vec3 m_colour;
    glm::vec3 m_ambient;
    glm::vec3 m_diffuse;
    glm::vec3 m_specular;
    glm::vec3 m_direction;
    bool m_gamma;
};

class DirLight: public Light {
public:
    DirLight(glm::vec3 position={0.0f, -1.0f, 0.0f},
             glm::vec3 colour={1.0f, 1.0f, 1.0f},
             bool gamma=false)
    :   Light {position, colour, gamma}
    {
        m_direction = -glm::normalize(m_position);
    }

    void computeLight(sjd::Shader& shader, [[maybe_unused]] unsigned int id=0) const {
        shader.setVec3("dirLight.direction", m_direction);
        shader.setVec3("dirLight.ambient", m_ambient*m_colour);
        shader.setVec3("dirLight.diffuse", m_diffuse*m_colour);
        shader.setVec3("dirLight.specular", m_specular*m_colour);
    }

private:
    glm::vec3 m_direction;
};

class PointLight: public Light {
public:
    PointLight(glm::vec3 position={0.0f, 0.0f, 0.0f},
               glm::vec3 colour={1.0f, 1.0f, 1.0f},
               bool gamma=false)
    :   Light {position, colour, gamma}
    ,   m_lightCubeShader {"../code/shaders/3.3.simple.vert.glsl", "../code/shaders/light_cube.frag.glsl"}
    {
        if (!gamma) {
            m_constant = 1.0f;
            m_linear = 0.09f;
            m_quadratic = 0.032f;
        }
        else {
            // for now these will do for both gamma and no
            m_constant = 1.0f;
            m_linear = 0.14f;
            m_quadratic = 0.07f;
        }

        glGenBuffers(1, &m_lightCubeVbo);
        glGenVertexArrays(1, &m_lightCubeVao);
        glBindVertexArray(m_lightCubeVao);
        glBindBuffer(GL_ARRAY_BUFFER, m_lightCubeVbo);
        glVertexAttribPointer(0,
                              3,
                              GL_FLOAT,
                              GL_FALSE,
                              3 * sizeof(float),
                              (void*)0);

        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(lightCubeVertices),
                     lightCubeVertices.data(),
                     GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glEnableVertexAttribArray(0);
    }

    void computeLight(sjd::Shader& shader, unsigned int id=0) const {
        shader.use();
        shader.setVec3(("pointLights[" + std::to_string(id) + "].position").c_str(), m_position);
        shader.setVec3(("pointLights[" + std::to_string(id) + "].ambient").c_str(), m_ambient*m_colour);
        shader.setVec3(("pointLights[" + std::to_string(id) + "].diffuse").c_str(), m_diffuse*m_colour);
        shader.setVec3(("pointLights[" + std::to_string(id) + "].specular").c_str(), m_specular*m_colour);
        shader.setFloat(("pointLights[" + std::to_string(id) + "].constant").c_str(), m_constant);
        shader.setFloat(("pointLights[" + std::to_string(id) + "].linear").c_str(), m_linear);
        shader.setFloat(("pointLights[" + std::to_string(id) + "].quadratic").c_str(), m_quadratic);
    }

    void drawLightCube(glm::mat4 projection, glm::mat4 view) {
        m_lightCubeShader.use();
        m_lightCubeShader.setMat4("projection", projection);
        m_lightCubeShader.setMat4("view", view);
        m_lightCubeShader.setMat4("model", glm::scale(glm::translate(glm::mat4(1.0f), m_position), glm::vec3(0.25f)));
        m_lightCubeShader.setVec3("lightColour", m_colour);
        glBindVertexArray(m_lightCubeVao);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }

    void moveTo(glm::vec3 position) {
        m_position = position;
    }

private:
    float m_constant;
    float m_linear;
    float m_quadratic;
    unsigned int m_lightCubeVao;
    unsigned int m_lightCubeVbo;
    sjd::Shader m_lightCubeShader;
};

}
#endif
