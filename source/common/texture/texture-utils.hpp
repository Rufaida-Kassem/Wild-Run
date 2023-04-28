#pragma once

#include "texture2d.hpp"
#include <string>

#include <glad/gl.h>
#include <glm/vec2.hpp>

namespace our::texture_utils
{
    // This function create an empty texture with a specific format (useful for framebuffers)
    // @param format: the format of the texture
    // @param size: the size of the texture
    // @return: the created texture
    Texture2D *empty(GLenum format, glm::ivec2 size);
    // This function loads an image and sends its data to the given Texture2D
    // @param filename: the path to the image file
    // @param generate_mipmap: if true, the function will generate the mipmap for the texture
    // @return: the loaded texture
    Texture2D *loadImage(const std::string &filename, bool generate_mipmap = true);
}