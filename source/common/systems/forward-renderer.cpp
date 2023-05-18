#include "forward-renderer.hpp"
#include "../mesh/mesh-utils.hpp"
#include "../texture/texture-utils.hpp"
// #include <iostream>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;
namespace our
{

    void ForwardRenderer::initialize(glm::ivec2 windowSize, const nlohmann::json &config)
    {
        // First, we store the window size for later use
        this->windowSize = windowSize;

        // Then we check if there is a sky texture in the configuration
        if (config.contains("sky"))
        {
            // First, we create a sphere which will be used to draw the sky
            this->skySphere = mesh_utils::sphere(glm::ivec2(16, 16));

            // We can draw the sky using the same shader used to draw textured objects
            ShaderProgram *skyShader = new ShaderProgram();
            skyShader->attach("assets/shaders/textured.vert", GL_VERTEX_SHADER);
            skyShader->attach("assets/shaders/textured.frag", GL_FRAGMENT_SHADER);
            skyShader->link();

            // TODO: (Req 10) Pick the correct pipeline state to draw the sky
            //  Hints: the sky will be draw after the opaque objects so we would need depth testing but which depth funtion should we pick?
            //  We will draw the sphere from the inside, so what options should we pick for the face culling.
            PipelineState skyPipelineState{};

            // TODO implementation
            /// set the configuration of the pipeline state for the sky so that it will be drawn after the opaque objects
            skyPipelineState.depthTesting.enabled = true;       // true as we want to use depth testing
            skyPipelineState.depthTesting.function = GL_LEQUAL; // LEQUAL --> less or equal --> if the new depth is less than or equal the old depth then we will draw the new one
            skyPipelineState.faceCulling.enabled = true;        // true as we want to draw the sphere from the inside
            skyPipelineState.faceCulling.culledFace = GL_FRONT; // we will remove the front face to show the sphere from inside

            // Load the sky texture (note that we don't need mipmaps since we want to avoid any unnecessary blurring while rendering the sky)
            std::string skyTextureFile = config.value<std::string>("sky", "");
            Texture2D *skyTexture = texture_utils::loadImage(skyTextureFile, false);

            // Setup a sampler for the sky
            Sampler *skySampler = new Sampler();
            skySampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            skySampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            skySampler->set(GL_TEXTURE_WRAP_S, GL_REPEAT);
            skySampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Combine all the aforementioned objects (except the mesh) into a material
            this->skyMaterial = new TexturedMaterial();
            this->skyMaterial->shader = skyShader;
            this->skyMaterial->texture = skyTexture;
            this->skyMaterial->sampler = skySampler;
            this->skyMaterial->pipelineState = skyPipelineState;
            this->skyMaterial->tint = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            this->skyMaterial->alphaThreshold = 1.0f;
            this->skyMaterial->transparent = false;
        }

        // Then we check if there is a postprocessing shader in the configuration
        if (config.contains("postprocess"))
        {
            // TODO: (Req 11) Create a framebuffer
            //. generation of framebuffer object
            //.  n=1 --> generates only one framebuffer object
            //.  *framebuffers = &postprocessFrameBuffer --> stores the framebuffers object
            glCreateFramebuffers(1, &postprocessFrameBuffer);
            // we use GL_DRAW_FRAMEBUFFER so that we can draw
            //. bind the framebuffer.
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postprocessFrameBuffer);
            //  TODO: (Req 11) Create a color and a depth texture and attach them to the framebuffer
            //  Hints: The color format can be (Red, Green, Blue and Alpha components with 8 bits for each channel).
            //  The depth format can be (Depth component with 24 bits).

            //. create an empty color texture to be the target of the color
            colorTarget = our::texture_utils::empty(GL_RGBA8, windowSize);
            //.  attaches the texture image specified by texture and level as one of the logical buffers of the currently bound framebuffer object.
            //.  attachment specifies whether the texture image should be attached to the framebuffer object's color, depth, or stencil buffer.
            //.  attach a texture image to a framebuffer object
            //.  target = GL_DRAW_FRAMEBUFFER so we can draw
            //.  attachment = GL_COLOR_ATTACHMENT0 --> attach the color
            //.  texturetarget = GL_TEXTURE_2D
            //.  texture = colorTarget->getOpenGLName()--> determine the texture color object name
            //.  level = 0 --> the mipmap level of the texture image to be attached and must be 0
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                   colorTarget->getOpenGLName(), 0);

            // create an empty depth texture to be the target of the depth
            depthTarget = our::texture_utils::empty(GL_DEPTH_COMPONENT24, windowSize);
            //. attach a texture image to a framebuffer object
            //. @param target = GL_DRAW_FRAMEBUFFER so we can draw
            //. @param attachment = GL_DEPTH_ATTACHMENT --> attach to the depth buffer
            //. @param texturetarget = GL_TEXTURE_2D
            //. @param texture = depthTarget->getOpenGLName()--> determine the texture depth object whose image is to be attached
            //. @param level = 0 -->  the mipmap level of the texture image to be attached and must be
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                                   depthTarget->getOpenGLName(), 0);

            // TODO: (Req 11) Unbind the framebuffer just to be safe
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

            // Create a vertex array to use for drawing the texture
            glGenVertexArrays(1, &postProcessVertexArray);

            // Create a sampler to use for sampling the scene texture in the post processing shader
            Sampler *postprocessSampler = new Sampler();
            postprocessSampler->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            postprocessSampler->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            postprocessSampler->set(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            postprocessSampler->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            // Create the post processing shader
            ShaderProgram *postprocessShader = new ShaderProgram();
            postprocessShader->attach("assets/shaders/fullscreen.vert", GL_VERTEX_SHADER);
            postprocessShader->attach(config.value<std::string>("postprocess", ""), GL_FRAGMENT_SHADER);
            postprocessShader->link();

            // Create a post-processing material
            postprocessMaterial = new TexturedMaterial();
            postprocessMaterial->shader = postprocessShader;
            postprocessMaterial->texture = colorTarget;
            postprocessMaterial->sampler = postprocessSampler;
            // The default options are fine but we don't need to interact with the depth buffer
            // so it is more performant to disable the depth mask
            postprocessMaterial->pipelineState.depthMask = false;
        }
    }

    void ForwardRenderer::destroy()
    {
        // Delete all objects related to the sky
        if (skyMaterial)
        {
            delete skySphere;
            delete skyMaterial->shader;
            delete skyMaterial->texture;
            delete skyMaterial->sampler;
            delete skyMaterial;
        }
        // Delete all objects related to post processing
        if (postprocessMaterial)
        {
            glDeleteFramebuffers(1, &postprocessFrameBuffer);
            glDeleteVertexArrays(1, &postProcessVertexArray);
            delete colorTarget;
            delete depthTarget;
            delete postprocessMaterial->sampler;
            delete postprocessMaterial->shader;
            delete postprocessMaterial;
        }
    }

    void ForwardRenderer::render(World *world)
    {
        // First of all, we search for a camera and for all the mesh renderers
        CameraComponent *camera = nullptr;
        opaqueCommands.clear();
        transparentCommands.clear();
        light_sources.clear();
        for (auto entity : world->getEntities())
        {
            // If we hadn't found a camera yet, we look for a camera in this entity
            if (!camera)
                camera = entity->getComponent<CameraComponent>();
            // If this entity has a mesh renderer component
            if (auto meshRenderer = entity->getComponent<MeshRendererComponent>(); meshRenderer)
            {
                // We construct a command from it
                RenderCommand command;
                command.localToWorld = meshRenderer->getOwner()->getLocalToWorldMatrix();
                command.center = glm::vec3(command.localToWorld * glm::vec4(0, 0, 0, 1));
                command.mesh = meshRenderer->mesh;
                command.material = meshRenderer->material;
                // if it is transparent, we add it to the transparent commands list
                if (command.material->transparent)
                {
                    transparentCommands.push_back(command);
                }
                else
                {
                    // Otherwise, we add it to the opaque command list
                    opaqueCommands.push_back(command);
                }
            }

            //. if this entity has a light component
            if (auto light = entity->getComponent<LightComponent>(); light)
            {
                //. if it is a sky light
                if (light->lightType == LightType::SKY)
                {

                    //. is enabled
                    sky_light_effect.isOn = light->isOn;
                    if (!light->isOn)
                    {
                        //. make the sky light effect black
                        sky_light_effect.top = glm::vec3(0, 1, 0);
                        sky_light_effect.horizon = glm::vec3(0, 0, 1);
                        sky_light_effect.bottom = glm::vec3(0, 1, 0);
                    }
                    else
                    {
                        //. we need to add the sky light effect
                        sky_light_effect.top = light->sky_top;
                        sky_light_effect.horizon = light->sky_middle;
                        sky_light_effect.bottom = light->sky_bottom;
                    }
                    continue;
                }

                //. if the light is off, we don't need to add it to the lights list
                if (!light->isOn)
                    continue;

                //. we add it to the lights list
                LightSource light_source = {};

                //. if the light is on
                light_source.isOn = light->isOn;

                //. we need to add the light position
                light_source.position = glm::vec3(light->getOwner()->getLocalToWorldMatrix() * glm::vec4(0, 0, 0, 1));

                //. we need to add the light color
                light_source.color = light->color;
                // //. we need to add the diffuse color
                // light_source.diffuse = light->diffuse;

                // //. we need to add the specular color
                // light_source.specular = light->specular;

                //. for the light type, we need to convert the enum to an int
                //. as the light type is an enum, we can cast it to an int
                light_source.type = static_cast<int>(light->lightType);
                light_source.attenuation = light->attenuation;

                //. check if the light is a spot light
                if (light->lightType == LightType::SPOT)
                {
                    //. we need to get the cone angles
                    light_source.cone_angles = glm::vec2(light->cone_angles.x, light->cone_angles.y);
                    light_source.direction = glm::vec3(light->getOwner()->getLocalToWorldMatrix() * glm::vec4(-1, 0, 0, 0));
                }
                else
                {
                    //. we need to add the light direction

                    light_source.direction = glm::vec3(light->getOwner()->getLocalToWorldMatrix() * glm::vec4(0, 0, -1, 0));
                }

                //. we add the light source to the light sources list
                light_sources.push_back(&light_source);
            }
        }

        // If there is no camera, we return (we cannot render without a camera)
        if (camera == nullptr)
            return;

        // TODO: (Req 9) Modify the following line such that "cameraForward" contains a vector pointing the camera forward direction
        //  HINT: See how you wrote the CameraComponent::getViewMatrix, it should help you solve this one
        //. to get the camera forward vector, we need the -Z as
        //. it's the forward vector of the camera (the camera gaze direction)
        //. we use the camera's local to world matrix to transform the forward vector of the camera
        glm::vec3 cameraForward = camera->getOwner()->getLocalToWorldMatrix() * glm::vec4(0, 0, -1, 0.0);

        std::sort(transparentCommands.begin(), transparentCommands.end(),
                  [cameraForward](const RenderCommand &first, const RenderCommand &second)
                  {
                      // TODO: (Req 9) Finish this function
                      //  HINT: the following return should return true "first" should be drawn before "second".
                      //. we draw the transparent objects from far to near
                      //. the dot product between the center of the object and the camera forward vector gives
                      //. the projection of the object on the camera forward vector,
                      //. the bigger the dot product the farther the object
                      return glm::dot(first.center, cameraForward) > glm::dot(second.center, cameraForward);
                  });

        // TODO: (Req 9) Get the camera ViewProjection matrix and store it in VP
        glm::mat4 VP = camera->getProjectionMatrix(windowSize) * camera->getViewMatrix();
        // TODO: (Req 9) Set the OpenGL viewport using viewportStart and viewportSize
        glm::ivec2 viewportStart = glm::ivec2(0, 0);
        glm::ivec2 viewportSize = windowSize;
        glViewport(viewportStart.x, viewportStart.y, viewportSize.x, viewportSize.y);

        // TODO: (Req 9) Set the clear color to black and the clear depth to 1
        glClearColor(0, 0, 0, 1);
        glClearDepth(1.0);

        // TODO: (Req 9) Set the color mask to true and the depth mask to true (to ensure the glClear will affect the framebuffer)
        glColorMask(true, true, true, true);
        glDepthMask(true);

        // If there is a postprocess material, bind the framebuffer so that we can render to it
        if (postprocessMaterial)
        {
            // TODO: (Req 11) bind the framebuffer
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postprocessFrameBuffer);
        }

        // TODO: (Req 9) Clear the color and depth buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // TODO: (Req 9) Draw all the opaque commands
        //  Don't forget to set the "transform" uniform to be equal the model-view-projection matrix for each render command
        //. what if the material is lightedMaterial?
        //. a special case for the lighted material
        // for (auto command: opaqueCommands) {
        //     if (auto lightedMaterial = dynamic_cast<LitMaterial *>(command.material); lightedMaterial) {

        for (auto command : opaqueCommands)
        {
            command.material->setup();

            //. if the material is lighted material
            if (auto lightedMaterial = dynamic_cast<LitMaterial *>(command.material); lightedMaterial)
            {
                //. try to send some value x
                command.material->shader->set("x", 5);
                //. send the camera position to the shader
                command.material->shader->set("camera_position", camera->getOwner()->getLocalToWorldMatrix());
                //. send the VP matrix to the shader
                command.material->shader->set("VP", VP);
                //. send the model matrix to the shader
                command.material->shader->set("M", command.localToWorld);
                //. send the inverse transpose of the model matrix to the shader
                command.material->shader->set("M_IT", glm::transpose(glm::inverse(command.localToWorld)));
                //. send the light sources count to the shader
                size_t light_sources_count = light_sources.size();
                //. send the light sources to the shader

                //. make the sky light effect black
                // sky_light_effect.top = glm::vec3(0, 1, 0);
                // sky_light_effect.horizon = glm::vec3(0, 0, 1);
                // sky_light_effect.bottom = glm::vec3(0, 1, 0);

                //. send the sky light effect to the shader
                command.material->shader->set("sky.top", sky_light_effect.top);
                command.material->shader->set("sky.horizon", sky_light_effect.horizon);
                command.material->shader->set("sky.bottom", sky_light_effect.bottom);

                //. single pass forward lighting approach
                //. send the light sources count to the shader
                // int light_sources_count = light_sources.size();
                command.material->shader->set("light_count", (int)light_sources_count);
                //. send the light sources to the shader
                for (size_t i = 0; i < light_sources.size(); i++)
                {
                    std::string light_sources_prefix = "lights[" + std::to_string(i) + "].";
                    command.material->shader->set(light_sources_prefix + "type", light_sources[i]->type);
                    command.material->shader->set(light_sources_prefix + "position", light_sources[i]->position);
                    command.material->shader->set(light_sources_prefix + "direction", light_sources[i]->direction);
                    command.material->shader->set(light_sources_prefix + "color", light_sources[i]->color);
                    command.material->shader->set(light_sources_prefix + "attenuation", light_sources[i]->attenuation);
                    command.material->shader->set(light_sources_prefix + "cone_angles", light_sources[i]->cone_angles);
                }
            }
            else
            {
                //. if the material is not lighted material
                command.material->shader->set("transform", VP * command.localToWorld);
            }
            command.mesh->draw();
        }

        // If there is a sky material, draw the sky
        if (this->skyMaterial)
        {
            // TODO: (Req 10) setup the sky material
            skyMaterial->setup(); // first we will setup the material

            // TODO: (Req 10) Get the camera position  // TO ASK
            /// why does the camera position is the origin?
            /// because the camera is at the origin of the world
            glm::vec3 cameraPosition =
                camera->getOwner()->getLocalToWorldMatrix() * glm::vec4(0, 0, 0, 1); // the camera eye is @ origin

            // TODO: (Req 10) Create a model matrix for the sky such that it always follows the camera (sky sphere center = camera position)
            /// then we will create a model matrix
            /// the translate function will create a matrix that will translate the sky sphere to the camera position
            /// so we give it the camera position
            /// and we give it a matrix of 1s so that it will not change the camera position
            glm::mat4 skyModelMat = glm::translate(
                glm::mat4(1.0f),
                cameraPosition);

            // TODO: (Req 10) We want the sky to be drawn behind everything (in NDC space, z=1) NDC --> Normalized Device Coordinates
            //  We can acheive the is by multiplying by an extra matrix after the projection but what values should we put in it?
            //  this mat4 will fill the matrix column by column
            /*
                1 0 0 0  --> keep x unchanged
                0 1 0 0  --> keep y unchanged
                0 0 0 1  --> let z = w
                0 0 0 1  --> keep w unchanged

                and then when dividing all by w (cause as we know we have to keep w = 1),
                we get z = 1
            */
            glm::mat4 alwaysBehindTransform = glm::mat4(
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 1.0f);

            // TODO: (Req 10) set the "transform" uniform
            skyMaterial->shader->set("transform", alwaysBehindTransform * camera->getProjectionMatrix(windowSize) *
                                                      camera->getViewMatrix() * skyModelMat);
            // model --> matrix for the sky as it transform from local space to world space
            // view --> matrix for the camera as it transform from world space to camera space
            // projection --> matrix for the camera as it transform from camera space to NDC space (canonical view volume) is this right?
            // alwaysBehindTransform --> matrix for the sky as it transform from NDC space to NDC space (always behind everything)

            // TODO: (Req 10) draw the sky sphere
            skySphere->draw(); // then we will draw the sky sphere
        }
        // TODO: (Req 9) Draw all the transparent commands
        //  Don't forget to set the "transform" uniform to be equal the model-view-projection matrix for each render command
        for (auto command : transparentCommands)
        {
            command.material->setup();
            command.material->shader->set("transform", VP * command.localToWorld);
            command.mesh->draw();
        }

        //. If there is a postprocess material, apply postprocessing then draw the fullscreen triangle to the screen
        //. note that we might want to change this behavior later
        if (postprocessMaterial)
        {
            // TODO: (Req 11) Return to the default framebuffer
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

            // TODO: (Req 11) Setup the postprocess material and draw the fullscreen triangle
            // we use the texture we rendered to as the input texture in a TexturedMaterial
            // we setup the material to apply the postprocess effect
            postprocessMaterial->setup();
            glBindVertexArray(postProcessVertexArray);

            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
        for (int i = 0; i < light_sources.size(); i++)
            std::cout << light_sources[i] << endl;
        // print light_sources to console
        // printf(light_sources[0]->type);
        // printf(light_sources[1]->type);
        //
    }
}