#pragma once

#include "../ecs/component.hpp"

#include <glm/glm.hpp>

namespace our {

    
    class PlayerComponent : public Component {
    public:
        
        // The ID of this component type is "Player"
        static std::string getID() { return "Player"; }

        void deserialize(const nlohmann::json& data) override;
    };

}