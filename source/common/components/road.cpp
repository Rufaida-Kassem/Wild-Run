#include "road.hpp"
#include "../ecs/entity.hpp"
#include "../deserialize-utils.hpp"

namespace our {
    // the follwoing function is overriden from the component class 
    // cause the road component inherits from the component class 
    // and this function is virtual in the component class 
    // and hence it must be overriden or else the road component will be an abstract class
    // but actually the road component is deserialized by the entity class after inserting it into the world
    void RoadComponent::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return; // if the data is not an object then return
    }
}