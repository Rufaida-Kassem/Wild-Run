#pragma once

#include "../ecs/component.hpp"

namespace our
{

    class CollisionComponent : public Component
    {
    public:
        float radius = 0.1; // Each frame, the entity should move as follows: position += linearVelocity * deltaTime

        // The ID of this component type is "Movement"
        static std::string getID() { return "Collision"; }

        // Reads linearVelocity & angularVelocity from the given json object
        void deserialize(const nlohmann::json &data) override
        {
            if (!data.is_object())
                return;
            radius = data.value("radius", radius);
        };
    };

}