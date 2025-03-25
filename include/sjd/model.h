#ifndef MODEL_H
#define MODEL_H

#define STB_IMAGE_IMPLEMENTATION    // configure stb headers before include
// Model class created following the instructions at learnopengl.com here:
// https://learnopengl.com/Model-Loading/Model

#include <string>
#include <vector>
#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <sjd/shader.h>
#include <sjd/mesh.h>
#include <stb/stb_image.h>

namespace sjd {
using VertsVec = std::vector<Mesh::Vertex>;
using TexVec = std::vector<Mesh::Texture>;

unsigned int TextureFromFile(const char *path, const std::string &directory);

class Model {
public:
    Model(std::string path) {
        loadModel(path);
    }

    void Draw(Shader &shader);	

private:
    // model data
    std::vector<Mesh> m_meshes;
    std::string m_directory;
    TexVec m_texturesLoaded;

    void loadModel(std::string path);

    void processNode(aiNode* node, const aiScene* scene);

    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

    TexVec loadMaterialTextures(aiMaterial* mat, aiTextureType type, 
                                         std::string typeName);
};

inline void Model::Draw(Shader &shader) {
    for(unsigned int i = 0; i < m_meshes.size(); i++)
        m_meshes[i].Draw(shader);
}

inline void Model::loadModel(std::string path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene -> mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene -> mRootNode) 
    {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }
    m_directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

inline void Model::processNode(aiNode* node, const aiScene* scene) {
    // process all the node's meshes (if any)
    for(unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene -> mMeshes[node -> mMeshes[i]]; 
        m_meshes.push_back(processMesh(mesh, scene));			
    }
    // then do the same for each of its children
    for(unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

inline Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    VertsVec vertices;
    TexVec textures;
    std::vector<unsigned int> indices;

    for(unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Mesh::Vertex vertex;
        // process vertex positions, normals and texture coordinates
        glm::vec3 vector; 

        vector.x = mesh -> mVertices[i].x;
        vector.y = mesh -> mVertices[i].y;
        vector.z = mesh -> mVertices[i].z; 
        vertex.position = vector;

        vector.x = mesh -> mNormals[i].x;
        vector.y = mesh -> mNormals[i].y;
        vector.z = mesh -> mNormals[i].z;
        vertex.normal = vector; 
        if (mesh -> mTextureCoords[0]) {
            glm::vec2 vec;
            vec.x = mesh -> mTextureCoords[0][i].x; 
            vec.y = mesh -> mTextureCoords[0][i].y;
            vertex.texCoords = vec;
        }
        else {
            vertex.texCoords = glm::vec2(0.0f, 0.0f);  
        }
        vertices.push_back(vertex);
    }
    // process indices

    for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for(unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // process material
    if(mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        TexVec diffuseMaps = loadMaterialTextures(material,
                                                  aiTextureType_DIFFUSE,
                                                  "texture_diffuse");
        textures.insert(textures.end(),
                        diffuseMaps.begin(),
                        diffuseMaps.end());
        TexVec specularMaps = loadMaterialTextures(material,
                                                   aiTextureType_SPECULAR,
                                                   "texture_specular");
        textures.insert(textures.end(),
                        specularMaps.begin(),
                        specularMaps.end());
    }

    return Mesh(vertices, indices, textures);
}

inline TexVec Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName) {
    TexVec textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        bool skip = false;
        for(unsigned int j = 0; j < m_texturesLoaded.size(); j++)
        {
            if(std::strcmp(m_texturesLoaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(m_texturesLoaded[j]);
                skip = true; 
                break;
            }
        }
        if(!skip)
        {   // if texture hasn't been loaded already, load it
            Mesh::Texture texture;
            texture.id = TextureFromFile(str.C_Str(), m_directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            m_texturesLoaded.push_back(texture); // add to loaded textures
        }
    }
    return textures;
}

inline unsigned int TextureFromFile(const char* path, const std::string &directory) {
    std::string filename(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format {};
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

}
#endif
