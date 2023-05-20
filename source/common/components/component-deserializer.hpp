#pragma once

#include "../ecs/entity.hpp"
#include "camera.hpp"
#include "mesh-renderer.hpp"
#include "free-camera-controller.hpp"
#include "movement.hpp"
#include "collision.hpp"
#include "road.hpp" /// phase 2
#include "coin.hpp"
#include "obstacle.hpp"
#include "monkey.hpp"
#include "cube.hpp"
#include "lightpole.hpp"

namespace our
{

    // Given a json object, this function picks and creates a component in the given entity
    // based on the "type" specified in the json object which is later deserialized from the rest of the json object
    inline void deserializeComponent(const nlohmann::json &data, Entity *entity)
    {
        std::string type = data.value("type", "");
        Component *component = nullptr;
        // TODO: (Req 8) Add an option to deserialize a "MeshRendererComponent" to the following if-else statement
        if (type == CameraComponent::getID())
        {
            component = entity->addComponent<CameraComponent>();
        }
        else if (type == FreeCameraControllerComponent::getID())
        {
            component = entity->addComponent<FreeCameraControllerComponent>();
        }
        else if (type == MovementComponent::getID())
        {
            component = entity->addComponent<MovementComponent>();
        }
        else if (type == MeshRendererComponent::getID())
        {
            component = entity->addComponent<MeshRendererComponent>();
        }
        else if (type == CollisionComponent::getID())
        {
            component = entity->addComponent<CollisionComponent>();
        }
        else if (type == RoadComponent::getID())
        { /// phase 2
            component = entity->addComponent<RoadComponent>();
        }
        else if (type == CoinComponent::getID())
        {
            component = entity->addComponent<CoinComponent>();
        }
        else if (type == MonkeyComponent::getID())
        {
            component = entity->addComponent<MonkeyComponent>();
        }
        else if (type == ObstacleComponent::getID())
        {
            component = entity->addComponent<ObstacleComponent>();
        }
        else if (type == LightPoleComponent::getID())
        {
            component = entity->addComponent<LightPoleComponent>();
        }
        else if (type == CubeComponent::getID())
        {
            component = entity->addComponent<CubeComponent>();
        }
        if (component)
            component->deserialize(data);
    }

}