#ifndef SCENE_H
#define SCENE_H

#include "sjd/framebuffer.h"
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

    void draw(sjd::Shader shader) {
        shader.use();
        shader.setVec3("viewPos", m_viewPos);
        if (m_dirLight) {
            m_dirLight->computeLight(shader);
        }
        shader.setInt("numPointLights", static_cast<int>(m_pointLights.size()));
        int i {0};
        for (std::reference_wrapper<sjd::PointLight> pointLight : m_pointLights) {
            pointLight.get().computeLight(shader, i++);
            pointLight.get().drawLightCube(m_projection, m_view);
        }
        for (std::reference_wrapper<sjd::Mesh> meshref : m_meshes) {
            meshref.get().draw(m_projection, m_view, shader);
        }
    }

    glm::mat4 m_projection;
    glm::mat4 m_view;
    glm::vec3 m_viewPos;
private:
    std::vector<std::reference_wrapper<sjd::Mesh>> m_meshes;
    std::vector<std::reference_wrapper<sjd::PointLight>> m_pointLights;
    sjd::DirLight* m_dirLight;
    sjd::FBTexture* m_shadowMap;

};

}
#endif
