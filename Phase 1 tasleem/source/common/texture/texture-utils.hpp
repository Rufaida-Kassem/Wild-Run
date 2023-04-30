#pragma once

#include "texture2d.hpp"
#include <string>

#include <glad/gl.h>
#include <glm/vec2.hpp>

namespace our::texture_utils
{
    // This function create an empty texture with a specific format (useful for framebuffers)
    // @param format: the format of the texture and must be one of the sized internal formats from the table 1 of the following link
    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glTexStorage2D.xhtml
    // @param size: the size of the texture
    // @return: the created texture
    Texture2D *empty(GLenum format, glm::ivec2 size);
    // This function loads an image and sends its data to the given Texture2D
    // @param filename: the path to the image file
    // @param generate_mipmap: if true, the function will generate the mipmap for the texture
    // @return: the loaded texture
    Texture2D *loadImage(const std::string &filename, bool generate_mipmap = true);
}