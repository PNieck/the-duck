#pragma once

#include <string>

#include <stb_image.h>

#include <glad/glad.h>


class CubeMap {
public:
    CubeMap(
        const std::string& posXTex,
        const std::string& negXTex,
        const std::string& posYTex,
        const std::string& negYTex,
        const std::string& posZTex,
        const std::string& negZTex
    );

    inline void Use() const {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    }

private:
    unsigned int textureID;

    void SetTexture(GLenum texType, const std::string& path) const;
};
