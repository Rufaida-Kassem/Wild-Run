#include "material.hpp"
#include <iostream>
#include "../asset-loader.hpp"
#include "deserialize-utils.hpp"

namespace our
{

    // This function should setup the pipeline state and set the shader to be used
    void Material::setup() const
    {
        // TODO: (Req 7) Write this function
        pipelineState.setup();
        shader->use();
    }

    // This function read the material data from a json object
    void Material::deserialize(const nlohmann::json &data)
    {
        if (!data.is_object())
            return;

        if (data.contains("pipelineState"))
        {
            pipelineState.deserialize(data["pipelineState"]);
        }
        shader = AssetLoader<ShaderProgram>::get(data["shader"].get<std::string>());
        transparent = data.value("transparent", false);
    }

    // This function should call the setup of its parent and
    // set the "tint" uniform to the value in the member variable tint
    void TintedMaterial::setup() const
    {
        // TODO: (Req 7) Write this function
        // call the setup of the parent
        Material::setup();
        // set the "tint" uniform to the value in the member variable tint
        shader->set("tint", tint);
    }

    // This function read the material data from a json object
    void TintedMaterial::deserialize(const nlohmann::json &data)
    {
        Material::deserialize(data);
        if (!data.is_object())
            return;
        tint = data.value("tint", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    }

    // This function should call the setup of its parent and
    // set the "alphaThreshold" uniform to the value in the member variable alphaThreshold
    // Then it should bind the texture and sampler to a texture unit and send the unit number to the uniform variable "tex"
    void TexturedMaterial::setup() const
    {
        // TODO: (Req 7) Write this function
        TintedMaterial::setup();

        // we need to set the alphaThreshold uniform so that we can use it in the shader to discard pixels (Alpha Testing)
        shader->set("alphaThreshold", alphaThreshold);

        // we will use UNIT_0 in the next bindings
        glActiveTexture(GL_TEXTURE0);
        if (texture != nullptr)
            texture->bind();

        if (sampler != nullptr)
            sampler->bind(0);

        // unit number to the shader
        shader->set("tex", 0);
    }

    // This function read the material data from a json object
    void TexturedMaterial::deserialize(const nlohmann::json &data)
    {
        TintedMaterial::deserialize(data);
        if (!data.is_object())
            return;
        alphaThreshold = data.value("alphaThreshold", 0.0f);
        texture = AssetLoader<Texture2D>::get(data.value("texture", ""));
        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));
    }

    // This function read the material data from a json object
    void LitMaterial::deserialize(const nlohmann::json &data)
    {
        Material::deserialize(data);
        if (!data.is_object())
            return;
        sampler = AssetLoader<Sampler>::get(data.value("sampler", ""));
        albedo = AssetLoader<Texture2D>::get(data.value("albedo", ""));
        specular = AssetLoader<Texture2D>::get(data.value("specular", ""));
        roughness = AssetLoader<Texture2D>::get(data.value("roughness", ""));
        emissive = AssetLoader<Texture2D>::get(data.value("emissive", ""));
        ambient_occlusion = AssetLoader<Texture2D>::get(data.value("ambient_occlusion", ""));
    }

    // This function should call the setup of its parent and
    // bind the light texture and sampler to a texture unit and send the unit number to the uniform variable "lightTex"
    void LitMaterial::setup() const
    {
        Material::setup();
        //. bind the albdeo, roughness, emissive, ambient_occlusion and specular textures to texture units
        //. and send the unit number to the uniform variables "material.albedo", "material.roughness", "material.emissive", "material.ambient_occlusion" and "material.specular"
        
        if (albedo != nullptr)
        {  
            glActiveTexture(GL_TEXTURE0);
            albedo->bind();
            sampler->bind(0);
            shader->set("material.albedo", 0);
        }
        if (roughness != nullptr)
        {
            glActiveTexture(GL_TEXTURE3);
            roughness->bind();
            sampler->bind(3);
            shader->set("material.roughness", 3);
        }
        if (emissive != nullptr)
        {
            glActiveTexture(GL_TEXTURE2);
            emissive->bind();
            sampler->bind(2);
            shader->set("material.emissive", 2);
        }
        if (ambient_occlusion != nullptr)
        {
            glActiveTexture(GL_TEXTURE4);
            ambient_occlusion->bind();
            sampler->bind(4);
            shader->set("material.ambient_occlusion", 4);
        }
        if (specular != nullptr)
        {
            glActiveTexture(GL_TEXTURE1);
            specular->bind();
            sampler->bind(1);
            shader->set("material.specular", 1);
        }
        // glActiveTexture(GL_TEXTURE0);
    }

}