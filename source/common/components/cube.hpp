#pragma once

#include "../ecs/component.hpp"

#include <glm/glm.hpp>

namespace our
{

    // this is a cube component to read it from the josn file and hence we can apply operations on the cubes
    // such as collision, lightening and redrawing cause our game is infinite
    class CubeComponent : public Component
    {
    public:
        bool collided = false;

        // The ID of this component type is "Cube"
        static std::string getID() { return "Cube"; }

        // this is the decleratoin of the deserialize function
        void deserialize(const nlohmann::json &data) override;
    };

}