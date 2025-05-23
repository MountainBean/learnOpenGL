#ifndef SCENE_H
#define SCENE_H

#include "sjd/framebuffer.h"
#include "sjd/skybox.h"
#include <functional>
#include <sjd/meshes/mesh.h>
#include <glm/glm.hpp>
#include <vector>
#include <sjd/light.h>
#include <sjd/shader.h>

namespace sjd {

class Scene {
public:

    Scene(std::vector<std::reference_wrapper<sjd::Mesh>> meshes)
    : m_meshes {meshes}
    , m_dirLight {nullptr}
    , m_shadowMap {nullptr}
    {
    }

    void addMesh(sjd::Mesh& mesh) {
        m_meshes.push_back(std::ref(mesh));
    }

    void setDirLight(sjd::DirLight* dirLight) {
        m_dirLight = dirLight;
        if (m_dirLight->isShadowMapEnabled()) {
            setShadowMap(m_dirLight->m_shadowMap);
        }
    }

    void setPointLights(std::vector<std::reference_wrapper<sjd::PointLight>> lights) {
        m_pointLights = lights;
    }

    void setShadowMap(sjd::FBTexture* shadowMap) {
        m_shadowMap = shadowMap;
        for (std::reference_wrapper<sjd::Mesh> meshref : m_meshes) {
            meshref.get().setShadowMap(shadowMap);
        }
    }

    void setSkyBox(sjd::Skybox* skybox) {
        m_skybox = skybox;
    }

    void draw(sjd::Shader shader) {
        shader.use();
        shader.setVec3("viewPos", m_viewPos);
        if (m_dirLight) {
            if (m_dirLight->isShadowMapEnabled()) {
                glm::mat4 lightSpaceMatrix = m_dirLight->generateLightSpaceMat();
                m_dirLight->bindDepthMap();
                _draw_objects(*m_dirLight->m_shadowMapShader);
                m_dirLight->unbindDepthMap();
                shader.use();
                shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
            }
            else {
                shader.use();
                shader.setMat4("lightSpaceMatrix", glm::mat4(1.0f));
            }
            m_dirLight->computeLight(shader);
        }

        shader.setInt("numPointLights", static_cast<int>(m_pointLights.size()));
        int i {0};
        for (std::reference_wrapper<sjd::PointLight> pointLight : m_pointLights) {
            pointLight.get().computeLight(shader, i++);
            pointLight.get().drawLightCube(m_projection, m_view);
        }

        _draw_objects(shader);

        if (m_skybox) {
            m_skybox->draw(m_projection, m_view);
        }
    }
private:
    void _draw_objects(sjd::Shader shader) {
        for (std::reference_wrapper<sjd::Mesh> meshref : m_meshes) {
            meshref.get().draw(m_projection, m_view, shader);
        }
    }

public:
    glm::mat4 m_projection;
    glm::mat4 m_view;
    glm::vec3 m_viewPos;
private:
    std::vector<std::reference_wrapper<sjd::Mesh>> m_meshes;
    std::vector<std::reference_wrapper<sjd::PointLight>> m_pointLights;
    sjd::DirLight* m_dirLight;
    sjd::FBTexture* m_shadowMap;
    sjd::Skybox* m_skybox;

};

}
#endif
