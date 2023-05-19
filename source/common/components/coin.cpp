#include "Coin.hpp"
#include "../ecs/entity.hpp"
#include "../deserialize-utils.hpp"

namespace our {
    
    // deserialize the coin from the josn file
    void CoinComponent::deserialize(const nlohmann::json& data){
        if(!data.is_object()) return;
    }
}