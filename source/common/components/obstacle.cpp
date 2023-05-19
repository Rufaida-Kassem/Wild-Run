#include "obstacle.hpp"
#include "../ecs/entity.hpp"
#include "../deserialize-utils.hpp"

namespace our {
    
    // deserialize the obstacle from the josn file
    void ObstacleComponent::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;
        
    }
}