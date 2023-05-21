#pragma once

#include "../ecs/component.hpp"

#include <glm/glm.hpp>

namespace our {

    // this is a light pole component
    // we need this component so that we can apply operation on it such as redrawing
    class LightPoleComponent : public Component {
    public:
        // The ID of this component type is "Obstacle"
        static std::string getID() { return "lightpole"; }

        // this is the decleratoin of the deserialize function
        void deserialize(const nlohmann::json& data) override;
    };

}