#ifndef MESH_H
#define MESH_H

#include <sjd/texture.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>

#include <sjd/shader.h>
#include <sjd/light.h>
#include <vector>


namespace sjd {

class Mesh {

protected:

    Mesh(sjd::Shader& shader)
    : m_model {1.0f}
    , m_shininess {32.0f}
    , m_diffuseMap {nullptr}
    , m_specularMap {nullptr}
    , m_shader {shader}
    {
        m_shader.use();
        m_shader.setFloat("material.shininess", m_shininess);
    }

public:

    virtual unsigned int defineVAOPointers() = 0;

    virtual void bufferData() = 0;

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

    void computeLight(const sjd::Light& light, glm::vec3 viewPos) {
        m_shader.use();
        m_shader.setVec3("viewPos", viewPos);
        light.computeLight(m_shader);
    }

    void computeLight(const std::vector<sjd::PointLight>& pointLights, glm::vec3 viewPos) {
        m_shader.use();
        m_shader.setVec3("viewPos", viewPos);
        m_shader.setInt("numPointLights", static_cast<int>(pointLights.size()));
        unsigned int i = 0;
        for (const sjd::PointLight& pointLight : pointLights) {
            pointLight.computeLight(m_shader, i);
            ++i;
        }
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
            glActiveTexture(GL_TEXTURE0 + m_specularMap->m_texNr);
            glBindTexture(GL_TEXTURE_2D, m_specularMap->m_id);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_specularMap->m_textureWrapS);	
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_specularMap->m_textureWrapT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_specularMap->m_textureMinFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_specularMap->m_textureMagFilter);
        }
        glBindVertexArray(m_vao);
        m_shader.use();
        m_shader.setMat4("projection", projection);
        m_shader.setMat4("view", view);
        m_shader.setMat4("model", m_model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }

protected:
    unsigned int m_vao;
    unsigned int m_vbo;
    glm::mat4 m_model;
    float m_shininess;
    sjd::Texture* m_diffuseMap;
    sjd::Texture* m_specularMap;
    sjd::Shader& m_shader;
};

}
#endif
