#include "Coin.hpp"
#include "../ecs/entity.hpp"
#include "../deserialize-utils.hpp"

namespace our {
    
    // the follwoing function is overriden from the component class 
    // cause the coin component inherits from the component class 
    // and this function is virtual in the component class 
    // and hence it must be overriden or else the coin component will be an abstract class
    // but actually the coin component is deserialized by the entity class after inserting it into the world
    void CoinComponent::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;
    }
}