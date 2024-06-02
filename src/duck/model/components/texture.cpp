#include <duck/model/components/texture.hpp>

#include <stdexcept>

#include <stb_image.h>


Texture::Texture(const std::string &path):
    Texture()
{
    int texWidth, texHeight, channels;
    unsigned char* data = stbi_load(path.c_str(), &texWidth, &texHeight, &channels, 0);
    if (data == NULL)
        throw std::runtime_error("Error while loading a texture");

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        texWidth,
        texHeight,
        0,          // zero for legacy reasons
        GL_RGB,
        GL_UNSIGNED_BYTE,
        data
    );
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}
