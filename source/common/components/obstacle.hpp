#pragma once

#include "../ecs/component.hpp"

#include <glm/glm.hpp>

namespace our {

    // this is an obstacle component
    // we need this component so that we can apply operation on it such as redrawing,
    // exist form the game once a player hits it
    class ObstacleComponent : public Component {
    public:
        // The ID of this component type is "Obstacle"
        static std::string getID() { return "Obstacle"; }

        void deserialize(const nlohmann::json& data) override;
    };

}