#pragma once

#include <glad/gl.h>

namespace our
{

    // This class defined an OpenGL texture which will be used as a GL_TEXTURE_2D
    class Texture2D
    {
        // The OpenGL object name of this texture
        GLuint name = 0;

    public:
        // This constructor creates an OpenGL texture and saves its object name in the member variable "name"
        Texture2D()
        {
            // TODO: (Req 5) Complete this function
            //. generate a texture object name
            glGenTextures(1, &name);
        };

        // This deconstructor deletes the underlying OpenGL texture
        ~Texture2D()
        {
            // TODO: (Req 5) Complete this function
            glDeleteTextures(1, &name);
        }

        // Get the internal OpenGL name of the texture which is useful for use with framebuffers
        // @return: the OpenGL name of the texture
        GLuint getOpenGLName()
        {
            return name;
        }

        //. This method binds this texture to GL_TEXTURE_2D
        //. NOTE: u should choose the active texture unit before calling this function
        //. we may want change this function in the future to take the target as a parameter if we want to create a different kind of textures (like GL_TEXTURE_3D or GL_TEXTURE_CUBE_MAP)
        void bind() const
        {
            // TODO: (Req 5) Complete this function
            glBindTexture(GL_TEXTURE_2D, name);
        }

        // This static method ensures that no texture is bound to GL_TEXTURE_2D
        // it unbinds the old texture obj if there is one
        static void unbind()
        {
            // TODO: (Req 5) Complete this function
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        Texture2D(const Texture2D &) = delete;
        Texture2D &operator=(const Texture2D &) = delete;
    };

}