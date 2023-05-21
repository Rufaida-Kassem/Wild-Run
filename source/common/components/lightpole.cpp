#include "lightpole.hpp"
#include "../ecs/entity.hpp"
#include "../deserialize-utils.hpp"

namespace our {
    
    // the follwoing function is overriden from the component class
    // it is used to deserialize the lightpole from the josn file
    // it takes the json object as a parameter
    // to deserialize the lightpole from the json file, the desrialize function of
    void LightPoleComponent::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;
        
    }
}