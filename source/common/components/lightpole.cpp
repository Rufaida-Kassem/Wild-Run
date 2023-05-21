#include "lightpole.hpp"
#include "../ecs/entity.hpp"
#include "../deserialize-utils.hpp"

namespace our {
    
    // the follwoing function is overriden from the component class 
    // cause the lightpole component inherits from the component class 
    // and this function is virtual in the component class 
    // and hence it must be overriden or else the light pole component will be an abstract class
    // but actually the light pole component is deserialized by the entity class after inserting it into the world
    void LightPoleComponent::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;
        
    }
}