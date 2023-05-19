#pragma once

#include "../ecs/component.hpp"

#include <glm/glm.hpp>

namespace our {

    // this is a coin component to read it from the josn file and hence we can apply operations on the coins
    // such as collision, lightening and redrawing cause our game is infinite
    class CoinComponent : public Component {
    public:
        bool collided = false;
        
        // The ID of this component type is "Coin"
        static std::string getID() { return "Coin"; }

        // this is the decleratoin of the deserialize function
        void deserialize(const nlohmann::json& data) override;
    };

}