#pragma once

#include <glad/gl.h>
#include <glm/vec4.hpp>
#include <json/json.hpp>

namespace our
{
    // There are some options in the render pipeline that we cannot control via shaders
    // such as blending, depth testing and so on
    // Since each material could require different options (e.g. transparent materials usually use blending and more complex materials may require more options),
    // we will encapsulate all these options into a single structure that will also be responsible for configuring OpenGL's pipeline
    struct PipelineState
    {
        // This set of pipeline options specifies whether face culling will be used or not and how it will be configured
        struct
        {
            bool enabled = false;
            GLenum culledFace = GL_BACK;  /// cullface --> the face to remove
            GLenum frontFace = GL_CCW; /// GL_CCW means counter-clockwise (direction of the front)
        } faceCulling;

        // This set of pipeline options specifies whether depth testing will be used or not and how it will be configured
        struct
        {
            bool enabled = false;
            GLenum function = GL_LEQUAL; /// LEQUAL --> less or equal --> if the new depth is less than or equal the old depth then we will draw the new one
        } depthTesting;

        // This set of pipeline options specifies whether blending will be used or not and how it will be configured
        struct
        {
            bool enabled = false;
            GLenum equation = GL_FUNC_ADD; /// GL_FUNC_ADD --> the new color will be added to the old one
            GLenum sourceFactor = GL_SRC_ALPHA; /// GL_SRC_ALPHA --> the old color will be multiplied by its alpha value (if it is transparent, it will be multiplied by 0 --> it will be invisible)
            GLenum destinationFactor = GL_ONE_MINUS_SRC_ALPHA; /// GL_ONE_MINUS_SRC_ALPHA --> the new color will be multiplied by (1 - the old color's alpha value) --> so tha if the source is transparent (its alpha = 0), the destination will be visible (multiplied by 1)
            glm::vec4 constantColor = {0, 0, 0, 0};
        } blending;

        // These options specify the color and depth mask which can be used to
        // prevent the rendering/clearing from modifying certain channels of certain targets in the framebuffer
        glm::bvec4 colorMask = {true, true, true, true}; // To know how to use it, check glColorMask
        bool depthMask = true;                           // To know how to use it, check glDepthMask

        // This function should set the OpenGL options to the values specified by this structure
        // For example, if faceCulling.enabled is true, you should call glEnable(GL_CULL_FACE), otherwise, you should call glDisable(GL_CULL_FACE)
        void setup() const
        {
            // TODO: (Req 4) Write this function
            if (faceCulling.enabled)
            {
                glEnable(GL_CULL_FACE);
                glCullFace(faceCulling.culledFace);
                glFrontFace(faceCulling.frontFace);
            }
            else
            {
                glDisable(GL_CULL_FACE);
            }

            if (depthTesting.enabled)
            {
                glEnable(GL_DEPTH_TEST);
                glDepthFunc(depthTesting.function);
            }
            else
            {
                glDisable(GL_DEPTH_TEST);
            }

            if (blending.enabled)
            {
                glEnable(GL_BLEND);
                glBlendEquation(blending.equation);
                glBlendFunc(blending.sourceFactor, blending.destinationFactor); /// parameters of the equation of adding the new color to the old one 
                glBlendColor(blending.constantColor.r, blending.constantColor.g, blending.constantColor.b, blending.constantColor.a); /// the constant color to be added to the new color
            }
            else
            {
                glDisable(GL_BLEND);
            }

            glColorMask(colorMask.r, colorMask.g, colorMask.b, colorMask.a);
            glDepthMask(depthMask);
        }

        // Given a json object, this function deserializes a PipelineState structure
        void deserialize(const nlohmann::json &data);
    };

}