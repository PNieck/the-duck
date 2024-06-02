#pragma once

#include <glad/glad.h>


class Texture {
public:
    Texture() {
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
    }

    inline void Use() const {
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    }

    inline void Update(int width, int height, GLenum type, const unsigned char * data)
        { glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, type, GL_UNSIGNED_BYTE, data); }

private:
    unsigned int textureID;
};