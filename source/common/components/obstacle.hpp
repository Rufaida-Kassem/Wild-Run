#pragma once

#include "../ecs/component.hpp"

#include <glm/glm.hpp>

namespace our {

    // this is an obstacle component
    // we need this component so that we can apply operation on it such as redrawing,
    // decide the lose condition and the score according to how many obstacles the player has hit
    class ObstacleComponent : public Component {
    public:
        bool collided = false; // this is a flag to check if the obstacle is collided with the car or not --> it will be needed by the obstacle controller system
        // The ID of this component type is "Obstacle"
        // this function is used to get the id of the component
        // so that we can deserialize it and insert it in the world entities
        static std::string getID() { return "Obstacle"; }

        // this is the decleratoin of the deserialize function
        void deserialize(const nlohmann::json& data) override;
    };

}