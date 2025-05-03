#ifndef QUAD_H
#define QUAD_H

#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
#include <sjd/texture.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>

#include <sjd/meshes/mesh.h>
#include <sjd/shader.h>
#include <sjd/light.h>
#include <vector>


namespace sjd {

class Quad: public Mesh {
public:
    Quad(sjd::Shader& shader,
         glm::vec3 bottomLeft={-1.0f, -1.0f, 0},
         glm::vec3 bottomRight={1.0f,-1.0f, 0},
         glm::vec3 topRight={1.0f, 1.0f, 0},
         glm::vec3 topLeft={-1.0f, 1.0f, 0})
    : Mesh {shader}
    {
        float texHeight {glm::length(topLeft - bottomLeft)};
        float texWidth {glm::length(bottomRight - bottomLeft)};
        glm::vec3 norm {glm::cross((bottomRight - bottomLeft),(topLeft - bottomLeft))};
        m_quadVertices = {
            bottomLeft.x, bottomLeft.y, bottomLeft.z,  norm.x,  norm.y, norm.z,  0.0f, 0.0f,
            bottomRight.x, bottomRight.y, bottomRight.z,  norm.x,  norm.y, norm.z,  texWidth, 0.0f,
            topRight.x,  topRight.y, topRight.z,  norm.x,  norm.y, norm.z,  texWidth, texHeight,
            topRight.x,  topRight.y, topRight.z,  norm.x,  norm.y, norm.z,  texWidth, texHeight,
            topLeft.x, topLeft.y, topLeft.z,  norm.x,  norm.y, norm.z,  0.0f, texHeight,
            bottomLeft.x, bottomLeft.y, bottomLeft.z,  norm.x,  norm.y, norm.z,  0.0f, 0.0f,
        };
        defineVAOPointers();
        bufferData();
        m_shader.use();
        m_shader.setFloat("material.shininess", m_shininess);
    }

        unsigned int defineVAOPointers() {
        glGenBuffers(1, &m_vbo);
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glVertexAttribPointer(0,
                              3,
                              GL_FLOAT,
                              GL_FALSE,
                              8 * sizeof(float),
                              (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1,
                              3,
                              GL_FLOAT,
                              GL_FALSE,
                              8 * sizeof(float),
                              (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2,
                              2,
                              GL_FLOAT,
                              GL_FALSE,
                              8 * sizeof(float),
                              (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        return m_vao;
    }

    void bufferData() {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(m_quadVertices),
                     m_quadVertices.data(),
                     GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    const std::array<float, 48>& getVertices() {
        return m_quadVertices;
    }

private:
    std::array<float, 48> m_quadVertices;
};

}
#endif
