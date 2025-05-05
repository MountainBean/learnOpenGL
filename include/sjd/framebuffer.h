#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <iostream>
#include <glad/glad.h>

namespace sjd {

class FBTexture {
public:

    FBTexture(unsigned int width, unsigned int height)
    : m_width {width}
    , m_height {height}
    , m_textureWrapS {GL_CLAMP_TO_BORDER}
    , m_textureWrapT {GL_CLAMP_TO_BORDER}
    , m_textureMinFilter {GL_NEAREST}
    , m_textureMagFilter {GL_NEAREST}
    {
        glGenFramebuffers(1, &m_fbo);
        glGenTextures(1, &m_id);
        glBindTexture(GL_TEXTURE_2D, m_id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                     width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_textureMinFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_textureMagFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_textureWrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_textureWrapT);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_id, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

    void bind() {
        glViewport(0, 0, m_width, m_height);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    }

    void release() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    unsigned int m_fbo;
    unsigned int m_id;
    unsigned int m_width;
    unsigned int m_height;
    unsigned int m_textureWrapS;
    unsigned int m_textureWrapT;
    unsigned int m_textureMinFilter;
    unsigned int m_textureMagFilter;

};
}
#endif
