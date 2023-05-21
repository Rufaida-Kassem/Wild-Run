#include "light.hpp"
#include "../ecs/entity.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "deserialize-utils.hpp"

namespace our
{
    //. Reads light parameters from the given json object
    void LightComponent::deserialize(const nlohmann::json &data)
    {
        if (!data.is_object())
            return;
        std::string lightTypeStr = data.value("lightType", "directional");

        //. if the light is on
        isOn = data.value("isOn", true);

        if (lightTypeStr == "directional")
        {
            lightType = LightType::DIRECTIONAL;
        }
        else if (lightTypeStr == "point")
        {
            lightType = LightType::POINT;
        } 
        else if (lightTypeStr == "sky")
        {
            lightType = LightType::SKY;
        }
        else if (lightTypeStr == "spot")
        {
            lightType = LightType::SPOT;
        }

        //. same for the three light types
        color = glm::vec3(data.value("color", glm::vec3(0.0f)));

        // intensity = data.value("intensity", 1.0f);
        
        // diffuse = glm::vec3(data.value("diffuse", glm::vec3(1.0f)));
        // specular = glm::vec3(data.value("specular", glm::vec3(1.0f)));

        //. for directional light, the ambient coefficient is always 1
        //. to decide later if we want to change it

        // if (lightType == LightType::DIRECTIONAL)
        // {
        //     ambient = glm::vec3(1.0f);
        // }
        if (lightType != LightType::DIRECTIONAL)
        {
            // ambient = glm::vec3(0.5f);
            // data.value("ambient", glm::vec3(0.0f));
            attenuation = data.value("attenuation", glm::vec3(1.0f, 0.0f, 0.0f));

        }
        
        //. to get the cone angles, we need to convert from degrees to radians
        //. the default values can be changed later
        if (lightType == LightType::SPOT)
        {
            cone_angles = glm::radians(data.value("cone_angles", glm::vec2(30.0f, 45.0f)));
        }

        //. for sky light, we need to get the three colors
        if (lightType == LightType::SKY)
        {
            sky_top = glm::vec3(data.value("sky_top", glm::vec3(0.0f)));
            sky_middle = glm::vec3(data.value("sky_middle", glm::vec3(0.0f)));
            sky_bottom = glm::vec3(data.value("sky_bottom", glm::vec3(0.0f)));
        }
    }
}