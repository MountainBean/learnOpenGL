#ifndef QUAD_H
#define QUAD_H

#include "glm/ext/vector_float3.hpp"
#include "glm/geometric.hpp"
#include <sjd/texture.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>

#include <sjd/shader.h>
#include <sjd/light.h>


namespace sjd {

class Quad {
public:
    Quad(sjd::Shader& shader,
         glm::vec3 bottomLeft={-1.0f, -1.0f, 0},
         glm::vec3 bottomRight={1.0f,-1.0f, 0},
         glm::vec3 topRight={1.0f, 1.0f, 0},
         glm::vec3 topLeft={-1.0f, 1.0f, 0})
    : m_model {1.0f}
    , m_shininess {32.0f}
    , m_diffuseMap {nullptr}
    , m_specularMap {nullptr}
    , m_shader {shader}
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
        bufferQuadData();
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

    const std::array<float, 48>& getVertices() {
        return m_quadVertices;
    }

    void bufferQuadData() {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(m_quadVertices),
                     m_quadVertices.data(),
                     GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void reset() {
        m_model = glm::mat4(1.0f);
    }

    void move(glm::vec3 location) {
        m_model = glm::translate(m_model, location);
    }

    void rotateX(float radians) {
        m_model = glm::rotate(m_model, radians, glm::vec3(1.0f, 0.0f, 0.0f));
    }

    void rotateY(float radians) {
        m_model = glm::rotate(m_model, radians, glm::vec3(0.0f, 1.0f, 0.0f));
    }

    void rotateZ(float radians) {
        m_model = glm::rotate(m_model, radians, glm::vec3(0.0f, 0.0f, 1.0f));
    }

    void scale(glm::vec3 scaleVec) {
        m_model = glm::scale(m_model, scaleVec);
    }

    void setDiffuseMap(sjd::Texture* diffuseMap) {
        m_shader.use();
        m_diffuseMap = diffuseMap;
        m_shader.setInt("material.diffuse", diffuseMap->m_texNr);
    }
    void setSpecularMap(sjd::Texture* specularMap) {
        m_shader.use();
        m_specularMap = specularMap;
        m_shader.setInt("material.specular", specularMap->m_texNr);
    }

    void computeLight(const sjd::DirLight& dirLight, glm::vec3 viewPos) {
        m_shader.use();
        m_shader.setVec3("viewPos", viewPos);
        dirLight.computeLight(m_shader);
    }

    void computeLight(const sjd::PointLight& pointLight, glm::vec3 viewPos) {
        m_shader.use();
        m_shader.setVec3("viewPos", viewPos);
        pointLight.computeLight(m_shader);
    }

    void draw(glm::mat4 projection, glm::mat4 view) {
        if (m_diffuseMap) {
            glActiveTexture(GL_TEXTURE0 + m_diffuseMap->m_texNr);
            glBindTexture(GL_TEXTURE_2D, m_diffuseMap->m_id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_diffuseMap->m_textureWrapS);	
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_diffuseMap->m_textureWrapT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_diffuseMap->m_textureMinFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_diffuseMap->m_textureMagFilter);

        }
        if (m_specularMap) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_specularMap->m_textureWrapS);	
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_specularMap->m_textureWrapT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_specularMap->m_textureMinFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_specularMap->m_textureMagFilter);
            glActiveTexture(GL_TEXTURE0 + m_specularMap->m_texNr);
            glBindTexture(GL_TEXTURE_2D, m_specularMap->m_id);
        }
        glBindVertexArray(m_vao);
        m_shader.use();
        m_shader.setMat4("projection", projection);
        m_shader.setMat4("view", view);
        m_shader.setMat4("model", m_model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }

private:
    unsigned int m_vao;
    unsigned int m_vbo;
    glm::mat4 m_model;
    float m_shininess;
    sjd::Texture* m_diffuseMap;
    sjd::Texture* m_specularMap;
    sjd::Shader& m_shader;
    std::array<float, 48> m_quadVertices;
};

}
#endif
