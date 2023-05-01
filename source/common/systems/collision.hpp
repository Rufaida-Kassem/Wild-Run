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
		int coins_collected = 0;
		bool is_lost = false;

	public:
		void OnInitialize()
		{
			is_lost = false;
			coins_collected = 0;
		}
		bool get_is_lost()
		{
			return is_lost;
		}
		int get_coins_collected()
		{
			return coins_collected;
		}

		bool AABBCollide(Entity* E1, Entity* E2)
		{
			CollisionComponent* comp1 = E1->getComponent<CollisionComponent>();
			CollisionComponent* comp2 = E2->getComponent<CollisionComponent>();
			glm::vec3 min1 = E1->getLocalToWorldMatrix() * glm::vec4(comp1->limit_min, 1);
			glm::vec3 max1 = E1->getLocalToWorldMatrix() * glm::vec4(comp1->limit_max, 1);
			glm::vec3 min2 = E2->getLocalToWorldMatrix() * glm::vec4(comp2->limit_min, 1);
			glm::vec3 max2 = E2->getLocalToWorldMatrix() * glm::vec4(comp2->limit_max, 1);



		}

		// This function is called every frame by the world to determine if there is any collision
		void update(World* world, float deltaTime)
		{
			// For each entity in the world
			std::vector<Entity*> entitiesToCollide;
			Entity* stick_index = nullptr;
			for (auto& entity : world->getEntities())
			{
				// Get the movement component if it exists
				CollisionComponent* coll = entity->getComponent<CollisionComponent>();
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
			Entity* entity1 = stick_index;
			// check if the stick is colliding with the any of the other entities
			for (auto& i : entitiesToCollide)
			{
				if (i == stick_index)
				{
					continue;
				}
				Entity* entity2 = i;
				// check if the two entities are colliding
				glm::vec4 entity1_pos = entity1->getLocalToWorldMatrix() * glm::vec4(0, 0, 0, 1);
				glm::vec4 entity2_pos = entity2->getLocalToWorldMatrix() * glm::vec4(0, 0, 0, 1);
				// check if the two entities are colliding
				float distance = 1; // entity2->getComponent<CollisionComponent>()->radius + entity1->getComponent<CollisionComponent>()->radius;

				if (glm::distance(entity1_pos, entity2_pos) < distance)
				{
					// if the two entities are colliding, then change delete the entity
					CollisionType type = entity2->getComponent<CollisionComponent>()->type;
					switch (type)
					{
					case CollisionType::COIN:
						coins_collected++;
						std::cout << coins_collected << std::endl;
						break;
					case CollisionType::OBSTACLE:
						is_lost = true;
						break;
					default:
						break;
					}

					world->markForRemoval(entity2);
				}
			}
		}
	};

}