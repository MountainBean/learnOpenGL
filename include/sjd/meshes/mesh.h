#ifndef MESH_H
#define MESH_H

#include "sjd/framebuffer.h"
#include <sjd/texture.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <array>

#include <sjd/shader.h>
#include <sjd/light.h>
#include <vector>


namespace sjd {
static unsigned int texNr {0};

class Mesh {

protected:
    struct TexPair {
        sjd::Texture* texture {nullptr};
        unsigned int textureUnit {};
    };

    struct FBTexPair {
        sjd::FBTexture* texture {nullptr};
        unsigned int textureUnit {};
    };

    Mesh()
    :   m_model {1.0f}
    ,   m_shininess {32.0f}
    {
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

    void scale(glm::vec3 scaling) {
        m_model = glm::scale(m_model, scaling);
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
        m_diffuseMap = {diffuseMap, texNr++};
    }

    void setSpecularMap(sjd::Texture* specularMap) {
        m_specularMap = {specularMap, texNr++};
    }

    void setShadowMap(sjd::FBTexture* shadowMap) {
        m_shadowMap = {shadowMap, texNr++};
    }

    virtual void draw(glm::mat4 projection, glm::mat4 view, sjd::Shader& shader) = 0;

protected:
    unsigned int m_vao;
    unsigned int m_vbo;
    glm::mat4 m_model;
    float m_shininess;
    TexPair m_diffuseMap;
    TexPair m_specularMap;
    FBTexPair m_shadowMap;
};

}
#endif
