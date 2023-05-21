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
        // this function is used to get the id of the component
        // so that we can deserialize it and insert it in the world entities
        static std::string getID() { return "Road"; }

        // this is the decleratoin of the deserialize function
        void deserialize(const nlohmann::json& data) override;
    };

}