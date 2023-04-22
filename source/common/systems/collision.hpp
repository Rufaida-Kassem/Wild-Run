#pragma once

#include "../ecs/world.hpp"
#include "../components/collision.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <vector>
namespace our
{

    void print_vec4(glm::vec4 v)
    {
        std::cout << v.x << " " << v.y << " " << v.z << " " << v.w << std::endl;
    }
    // The movement system is responsible for moving every entity which contains a MovementComponent.
    // This system is added as a simple example for how use the ECS framework to implement logic.
    // For more information, see "common/components/movement.hpp"
    class CollisionSystem
    {
    public:
        // This should be called every frame to update all entities containing a MovementComponent.
        void update(World *world, float deltaTime)
        {
            // For each entity in the world
            std::vector<Entity *> entitiesToCollide;
            Entity *stick_index = nullptr;
            for (auto &entity : world->getEntities())
            {
                // Get the movement component if it exists
                CollisionComponent *coll = entity->getComponent<CollisionComponent>();
                // If the movement component exists
                if (coll)
                {
                    entitiesToCollide.push_back(entity);
                }
                if (entity->name == "stick")
                {
                    stick_index = entity;
                }
            }

            if (!stick_index)
            {
                return;
            }
            Entity *entity1 = stick_index;
            // check if the stick is colliding with the any of the other entities
            for (auto &i : entitiesToCollide)
            {
                if (i == stick_index)
                {
                    continue;
                }
                Entity *entity2 = i;
                // check if the two entities are colliding
                glm::vec4 entity1_pos = entity1->getLocalToWorldMatrix() * glm::vec4(0, 0, 0, 1);
                glm::vec4 entity2_pos = entity2->getLocalToWorldMatrix() * glm::vec4(0, 0, 0, 1);
                // check if the two entities are colliding
                std ::cout << "entity1_pos" << std::endl;
                print_vec4(entity1_pos);
                std ::cout << "entity2_pos" << std::endl;
                print_vec4(entity2_pos);
                float distance = entity2->getComponent<CollisionComponent>()->radius + entity1->getComponent<CollisionComponent>()->radius;
                if (glm::distance(entity1_pos, entity2_pos) < distance)
                {
                    // if the two entities are colliding, then change delete the entity
                    std::cout << "collision " << std::endl;

                    world->markForRemoval(entity2);
                }
            }
        }
    };

}
