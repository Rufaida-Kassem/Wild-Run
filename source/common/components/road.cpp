#include "road.hpp"
#include "../ecs/entity.hpp"
#include "../deserialize-utils.hpp"

namespace our {
    void RoadComponent::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;
    }
}