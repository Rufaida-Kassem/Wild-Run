#include "texture-utils.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <iostream>

//. this function creates a new texture wit the given size and format
//. we will use it to create textures for framebuffers
//. @param format: the format of the texture
our::Texture2D *our::texture_utils::empty(GLenum format, glm::ivec2 size)
{
    // generate a texture object
    our::Texture2D *texture = new our::Texture2D();
    // TODO: (Req 11) Finish this function to create an empty texture with the given size and format
    // bind the texture so that we can configure it
    texture->bind();
    // glTexStorage2D and glTextureStorage2D specify the storage requirements for all levels of a two-dimensional texture
    // we will make our Texure with 1 level and the given format and size
    glTexStorage2D(GL_TEXTURE_2D, 1, format, size.x, size.y);

    return texture;
}

our::Texture2D *our::texture_utils::loadImage(const std::string &filename, bool generate_mipmap)
{
    glm::ivec2 size;
    int channels;
    // Since OpenGL puts the texture origin at the bottom left while images typically has the origin at the top left,
    // We need to tell stb to flip images vertically after loading them
    stbi_set_flip_vertically_on_load(true);
    // Load image data and retrieve width, height and number of channels in the image
    // The last argument is the number of channels we want and it can have the following values:
    //- 0: Keep number of channels the same as in the image file
    //- 1: Grayscale only
    //- 2: Grayscale and Alpha
    //- 3: RGB
    //- 4: RGB and Alpha (RGBA)
    // Note: channels (the 4th argument) always returns the original number of channels in the file
    unsigned char *pixels = stbi_load(filename.c_str(), &size.x, &size.y, &channels, 4);
    if (pixels == nullptr)
    {
        std::cerr << "Failed to load image: " << filename << std::endl;
        return nullptr;
    }
    // Create a texture
    our::Texture2D *texture = new our::Texture2D();
    // TODO: (Req 5) Finish this function to fill the texture with the data found in "pixels"

    //. Bind the texture such that we upload the image data to its storage
    texture->bind();
    //. sets pixel storage mode to unpack the data from the image
    //. i passed 1 as argument as it will work with all images
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    //. glTexImage2D specifies a two-dimensional texture image and the texture parameters
    //. load the image into level 0 of the texture then generate the mipmap if generate_mipmap is true
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.x, size.y, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, (void *)pixels);

    //. will generate the mipmap for the texture if generate_mipmap is true
    //. we might not want to generate the mipmap for some textures (like the skybox)
    if (generate_mipmap)
        glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(pixels); // Free image data after uploading to GPU
    return texture;
}