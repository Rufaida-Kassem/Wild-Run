#pragma once

#include "../ecs/component.hpp"

#include <glm/glm.hpp>

namespace our {

    // this is a coin component to read it from the josn file and hence we can apply operations on the coins
    // such as collision, lightening and redrawing cause our game is infinite
    class CoinComponent : public Component {
    public:
        bool collided = false; // this is a flag to check if the coin is collided with the car or not --> it will be needed by the coin controller system
        
        // The ID of this component type is "Coin"
        // this function is used to get the id of the component
        // so that we can deserialize it and insert it in the world entities
        static std::string getID() { return "Coin"; }

        // this is the decleratoin of the deserialize function
        void deserialize(const nlohmann::json& data) override;
    };

}