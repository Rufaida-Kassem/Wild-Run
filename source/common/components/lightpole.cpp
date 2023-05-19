#include "lightpole.hpp"
#include "../ecs/entity.hpp"
#include "../deserialize-utils.hpp"

namespace our {
    
    // deserialize the obstacle from the josn file
    void LightPoleComponent::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;
        
    }
}