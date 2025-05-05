#ifndef QUAD_H
#define QUAD_H

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
    Quad(glm::vec3 bottomLeft={-1.0f, -1.0f, 0},
         glm::vec3 bottomRight={1.0f,-1.0f, 0},
         glm::vec3 topRight={1.0f, 1.0f, 0},
         glm::vec3 topLeft={-1.0f, 1.0f, 0})
    : Mesh {}
    {
        float texHeight {glm::length(topLeft - bottomLeft)};
        float texWidth {glm::length(bottomRight - bottomLeft)};
        glm::vec3 norm {glm::normalize(glm::cross((bottomRight - bottomLeft),(topLeft - bottomLeft)))};
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
    }

        virtual unsigned int defineVAOPointers() {
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

    virtual void bufferData() {
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

    virtual void draw(glm::mat4 projection, glm::mat4 view, sjd::Shader& shader) {
        shader.use();
        shader.setFloat("material.shininess", m_shininess);
        if (m_diffuseMap.texture) {
            shader.setInt("material.diffuse", m_diffuseMap.textureUnit);
            glActiveTexture(GL_TEXTURE0 + m_diffuseMap.textureUnit);
            glBindTexture(GL_TEXTURE_2D, m_diffuseMap.texture->m_id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_diffuseMap.texture->m_textureWrapS);	
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_diffuseMap.texture->m_textureWrapT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_diffuseMap.texture->m_textureMinFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_diffuseMap.texture->m_textureMagFilter);
        }
        if (m_specularMap.texture) {
            shader.setInt("material.specular", m_specularMap.textureUnit);
            glActiveTexture(GL_TEXTURE0 + m_specularMap.textureUnit);
            glBindTexture(GL_TEXTURE_2D, m_specularMap.texture->m_id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_specularMap.texture->m_textureWrapS);	
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_specularMap.texture->m_textureWrapT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_specularMap.texture->m_textureMinFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_specularMap.texture->m_textureMagFilter);
        }
        if (m_shadowMap.texture) {
            shader.setInt("shadowMap", m_shadowMap.textureUnit);
            glActiveTexture(GL_TEXTURE0 + m_shadowMap.textureUnit);
            glBindTexture(GL_TEXTURE_2D, m_shadowMap.texture->m_id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_shadowMap.texture->m_textureWrapS);	
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_shadowMap.texture->m_textureWrapT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_shadowMap.texture->m_textureMinFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_shadowMap.texture->m_textureMagFilter);
            float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        }
        glBindVertexArray(m_vao);
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setMat4("model", m_model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

private:
    std::array<float, 48> m_quadVertices;
};

}
#endif
