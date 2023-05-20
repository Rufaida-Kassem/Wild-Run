#include "cube.hpp"
#include "../ecs/entity.hpp"
#include "../deserialize-utils.hpp"

namespace our
{

    // deserialize the cube from the josn file
    void CubeComponent::deserialize(const nlohmann::json &data)
    {
        if (!data.is_object())
            return;
    }
}