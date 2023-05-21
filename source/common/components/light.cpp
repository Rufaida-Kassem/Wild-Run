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

        //. to decide the light type
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

        //. same for the three light types (point, spot, directional)
        color = glm::vec3(data.value("color", glm::vec3(0.0f)));

        //. for point and spot lights, we need to get the attenuation
        if (lightType == LightType::POINT || lightType == LightType::SPOT)
        {
            attenuation = data.value("attenuation", glm::vec3(0.0f, 1.0f, 0.0f));

        }
        
        //. to get the cone angles, we need to convert from degrees to radians
        if (lightType == LightType::SPOT)
        {
            cone_angles = glm::radians(data.value("cone_angles", glm::vec2(30.0f, 45.0f)));
        }

        //. for sky light, we need to get the three colors
        //. as we will use the sky light as ambient light
        if (lightType == LightType::SKY)
        {
            sky_top = glm::vec3(data.value("sky_top", glm::vec3(0.0f)));
            sky_middle = glm::vec3(data.value("sky_middle", glm::vec3(0.0f)));
            sky_bottom = glm::vec3(data.value("sky_bottom", glm::vec3(0.0f)));
        }
    }
}