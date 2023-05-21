#include "Monkey.hpp"
#include "../ecs/entity.hpp"
#include "../deserialize-utils.hpp"

namespace our
{

    // deserialize the monkey from the josn file
    void MonkeyComponent::deserialize(const nlohmann::json &data)
    {
        if (!data.is_object())
            return;
    }
}