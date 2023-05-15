#include "light.hpp"
#include "../ecs/entity.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace our
{
    //. Reads light parameters from the given json object
    void LightComponent::deserialize(const nlohmann::json &data)
    {
        if (!data.is_object())
            return;
        std::string lightTypeStr = data.value("lightType", "directional");

        if (lightTypeStr == "directional")
        {
            lightType = LightType::DIRECTIONAL;
        }
        else if (lightTypeStr == "point")
        {
            lightType = LightType::POINT;
        }
        else
        {
            lightType = LightType::SPOT;
        }

        //. same for all light types
        color = glm::vec4(1.0f);
        // glm::vec4(data.value("color", glm::vec4(1.0f)));
        intensity = data.value("intensity", 1.0f);
        diffuse = glm::vec3(1.0f);
        // glm::vec3(data.value("diffuse", glm::vec3(1.0f)));
        specular = glm::vec3(1.0f);
        // glm::vec3(data.value("specular", glm::vec3(1.0f)));

        //. for directional light, the ambient coefficient is always 1
        //. to decide later if we want to change it

        if (lightType == LightType::DIRECTIONAL)
        {
            ambient = glm::vec3(1.0f);
        }
        else
        {
            ambient = glm::vec3(0.5f);
            // data.value("ambient", glm::vec3(0.0f));
            attenuation = glm::vec3(1.0f, 0.0f, 0.0f);
            // data.value("attenuation", glm::vec3(1.0f, 0.0f, 0.0f));

        }
        
        //. to get the cone angles, we need to convert from degrees to radians
        //. the default values can be changed later
        if (lightType == LightType::SPOT)
        {
            cone_angles = glm::radians(glm::vec2(30.0f, 45.0f));
            // glm::radians(data.value("cone_angles", glm::vec2(30.0f, 45.0f)));
        }

    }
}