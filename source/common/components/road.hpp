#pragma once

#include "../ecs/component.hpp"

#include <glm/glm.hpp>

namespace our {

    // this is the road compoenent 
    // we need it so that we will be able to do operatoin on the road and control it using a created system
    // As our game is infinite so that we will need to draw 2 roads and we will explain how we will do this
    // in the road controller system
    class RoadComponent : public Component {
    public:

        // The ID of this component type is "Road"
        static std::string getID() { return "Road"; }

        void deserialize(const nlohmann::json& data) override;
    };

}