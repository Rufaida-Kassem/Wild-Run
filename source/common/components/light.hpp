#pragma once

#include "../ecs/component.hpp"

#include <glm/mat4x4.hpp>

namespace our
{

    // An enum that defines the type of the light (DIRECTIONAL, POINT or SPOT)
    enum class LightType
    {
        DIRECTIONAL,
        POINT,
        SPOT,
        SKY
    };

    class LightComponent : public Component
    {
    public:
        LightType lightType; //. The type of the light
        bool isOn;           //. Whether the light is on or off
        glm::vec4 color;     //. The color of the light
        // float intensity;     //. The intensity of the light
        glm::vec3 diffuse;   //. The diffuse coefficient of the light
        glm::vec3 specular;  //. The specular coefficient of the light
        // glm::vec3 ambient;   //. The ambient coefficient of the light
        glm::vec3 attenuation; //. x*d^2 + y*d + z
        glm::vec2 cone_angles; //. x: inner_angle, y: outer_angle
        glm::vec3 sky_top;
        glm::vec3 sky_middle;
        glm::vec3 sky_bottom;
        //. Reads light parameters from the given json object
        void deserialize(const nlohmann::json& data) override;
        //. identify it is a light component
        static std::string getID() { return "light"; }
    };

}