#ifndef TEXTURE_H
#define TEXTURE_H

#include <iostream>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <glad/glad.h>

namespace sjd {
// static int to keep track of the texture for binding to shaders

class Texture {
public:

    Texture(const std::string& path, bool gamma=false) 
    : m_textureWrapS {GL_CLAMP_TO_EDGE}
    , m_textureWrapT {GL_CLAMP_TO_EDGE}
    , m_textureMinFilter {GL_LINEAR_MIPMAP_LINEAR}
    , m_textureMagFilter {GL_NEAREST}
    {

        glGenTextures(1, &m_id);
        int texWidth;
        int texHeight;
        int nrChannels;
        unsigned char* data = stbi_load(path.c_str(),
                                        &texWidth,
                                        &texHeight,
                                        &nrChannels,
                                        0);
        if (data) {
            GLenum formatOut {};
            GLenum formatIn {};
            if (nrChannels == 1) {
                formatOut = GL_RED;
                formatIn = GL_RED;
            }
            else if (nrChannels == 3) {
                formatOut = GL_RGB;
                (gamma) ? formatIn = GL_SRGB : formatIn = GL_RGB;
            }
            else if (nrChannels == 4) {
                formatOut = GL_RGBA;
                (gamma) ? formatIn = GL_SRGB_ALPHA : formatIn = GL_RGBA;
            }

            glBindTexture(GL_TEXTURE_2D, m_id);
            glTexImage2D(GL_TEXTURE_2D,
                         0,
                         formatIn,
                         texWidth,
                         texHeight,
                         0,
                         formatOut,
                         GL_UNSIGNED_BYTE,
                         data);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_textureWrapS);	
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_textureWrapT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_textureMinFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_textureMagFilter);


            glGenerateMipmap(GL_TEXTURE_2D);

            stbi_image_free(data);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        else {
            std::cout << "ERROR::TEXTURE::LOAD::NO_DATA" << std::endl;
        }
    }

    void setTextureParameter(unsigned int glTextureParameter, unsigned int glTextureDefinition) {
        switch (glTextureParameter)
        {
            case GL_TEXTURE_WRAP_S:
                m_textureWrapS = glTextureDefinition;
                return;
            case GL_TEXTURE_WRAP_T:
                m_textureWrapT = glTextureDefinition;
                return;
            case GL_TEXTURE_MIN_FILTER:
                m_textureMinFilter = glTextureDefinition;
                return;
            case GL_TEXTURE_MAG_FILTER:
                m_textureMagFilter = glTextureDefinition;
                return;
            default:
                return;
        }

    }

    unsigned int m_id;
    unsigned int m_textureWrapS;
    unsigned int m_textureWrapT;
    unsigned int m_textureMinFilter;
    unsigned int m_textureMagFilter;

};
}
#endif
