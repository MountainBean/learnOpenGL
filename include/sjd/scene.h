#ifndef SCENE_H
#define SCENE_H

#include <functional>
#include <glm/glm.hpp>
#include <vector>
#include <sjd/light.h>
#include <sjd/shader.h>
#include <sjd/meshes/mesh.h>

namespace sjd {
class Scene {
public:

    Scene(std::vector<std::reference_wrapper<sjd::Mesh>> meshes)
    : m_meshes {meshes}
    {
    }

    void addMesh(sjd::Mesh& mesh) {
        m_meshes.push_back(std::ref(mesh));
    }

    void setProjection(glm::mat4 projection) {
        m_projection = projection;
    }

    void setView(glm::mat4 view) {
        m_view = view;
    }

    void computeLight(sjd::Light& light, glm::vec3 viewPos) {
        for (std::reference_wrapper<sjd::Mesh> mesh : m_meshes) {
            mesh.get().computeLight(light, viewPos);
        }
    }

    void draw() {
        for (std::reference_wrapper<sjd::Mesh> mesh : m_meshes) {
            mesh.get().draw(m_projection, m_view);
        }
    }

private:
    std::vector<std::reference_wrapper<sjd::Mesh>> m_meshes;
    glm::mat4 m_projection;
    glm::mat4 m_view;

};

}
#endif
