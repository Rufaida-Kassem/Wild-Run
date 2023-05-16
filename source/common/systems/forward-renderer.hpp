#pragma once

#include "../ecs/world.hpp"
#include "../components/camera.hpp"
#include "../components/mesh-renderer.hpp"
#include "../components/light.hpp"
#include "../asset-loader.hpp"

#include <glad/gl.h>
#include <vector>
#include <algorithm>

namespace our
{

    // The render command stores command that tells the renderer that it should draw
    // the given mesh at the given localToWorld matrix using the given material
    // The renderer will fill this struct using the mesh renderer components
    struct RenderCommand
    {
        glm::mat4 localToWorld; // TRS matrix of the object
        glm::vec3 center;
        Mesh *mesh;
        Material *material;
    };

    //. this is a struct for lights like the one in the lightened.frag
    //. it is used to pass light data to the shader
    struct LightSource
    {
        int type;
        bool isOn;
        glm::vec3 position;
        glm::vec3 direction;
        glm::vec3 diffuse;
        glm::vec3 specular;
        glm::vec3 attenuation;
        glm::vec2 cone_angles;
    };

    //. this is for the sky light effect on objects
    //. it is used to pass sky light data to the shader
    struct SkyLightEffect
    {
        glm::vec3 top, horizon, bottom;
    };

    // A forward renderer is a renderer that draw the object final color directly to the framebuffer
    // In other words, the fragment shader in the material should output the color that we should see on the screen
    // This is different from more complex renderers that could draw intermediate data to a framebuffer before computing the final color
    // In this project, we only need to implement a forward renderer
    class ForwardRenderer
    {
        //. create light sources array and pass it to the shader
        //. it is an array of pointers to light components
        //. the size of the array is the number of light components
        std::vector<LightSource *> light_sources;
        // These window size will be used on multiple occasions (setting the viewport, computing the aspect ratio, etc.)
        glm::ivec2 windowSize;
        // These are two vectors in which we will store the opaque and the transparent commands.
        // We define them here (instead of being local to the "render" function) as an optimization to prevent reallocating them every frame
        std::vector<RenderCommand> opaqueCommands;
        std::vector<RenderCommand> transparentCommands;

        // Objects used for rendering a skybox
        // sky is just a sphere with a texture that is drawn behind everything else
        Mesh *skySphere;
        TexturedMaterial *skyMaterial; // to store its texture
        // Objects used for Postprocessing
        GLuint postprocessFrameBuffer, postProcessVertexArray;
        Texture2D *colorTarget, *depthTarget;
        TexturedMaterial *postprocessMaterial;

    public:
        // Initialize the renderer including the sky and the Postprocessing objects.
        // @param windowSize: the width & height of the window (in pixels).
        // @param config: the configuration of the renderer, it may contain the following:
        //      - sky: the path to the sky texture
        //      - postprocess: the path to the postprocessing shader
        //      - postprocessUniforms: a list of uniforms to set on the postprocessing shader
        //      - postprocessUniforms[i].name: the name of the uniform to set
        void initialize(glm::ivec2 windowSize, const nlohmann::json &config);
        // Clean up the renderer
        void destroy();
        // This function should be called every frame to draw the given world
        void render(World *world);
    };

}