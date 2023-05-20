#pragma once

#include "../ecs/world.hpp"
#include "../components/collision.hpp"
#include "../components/coin.hpp"
#include "../components/obstacle.hpp"
#include "../components/monkey.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <vector>
#include <algorithm>
#include <cstdlib>
// #include <cstdlib>

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
        int lives = 1110;
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

        int get_lives()
        {
            return lives;
        }

        //		bool AABBCollide(Entity* E1, Entity* E2)
        //		{
        //			auto* comp1 = E1->getComponent<CollisionComponent>();
        //			auto* comp2 = E2->getComponent<CollisionComponent>();
        //			glm::vec3 min1 = E1->getLocalToWorldMatrix() * glm::vec4(comp1->limit_min, 1);
        //			glm::vec3 max1 = E1->getLocalToWorldMatrix() * glm::vec4(comp1->limit_max, 1);
        //			glm::vec3 min2 = E2->getLocalToWorldMatrix() * glm::vec4(comp2->limit_min, 1);
        //			glm::vec3 max2 = E2->getLocalToWorldMatrix() * glm::vec4(comp2->limit_max, 1);
        //			// the lowest point of the first box is higher than the highest point of the second box
        //			// or the highest point of the first box is lower than the lowest point of the second box
        //			// print every 2 seconds
        //			/*std::cout << E1->name << std::endl;
        //			std::cout << "min1: ";
        //			print_vec4(glm::vec4(min1, 0));
        //			std::cout << "max1: ";
        //			print_vec4(glm::vec4(max1, 0));
        //			std::cout << E2->name << std::endl;
        //			std::cout << "min2: ";
        //			print_vec4(glm::vec4(min2, 0));
        //			std::cout << "max2: ";
        //			print_vec4(glm::vec4(max2, 0));*/
        //
        //			return min1.x < max2.x && max1.x > min2.x &&
        //				min1.y < max2.y && max1.y > min2.y &&
        //				min1.z < max2.z && max1.z > min2.z;
        //		}

        //        http://programmerart.weebly.com/separating-axis-theorem.html
        // to understand the algorithm
        // i did not copy the code from the website but i did use the idea of the algorithm to implement it in 3D

        static bool SatCollide(Entity *E1, Entity *E2)
        {
            auto *comp1 = E1->getComponent<CollisionComponent>();
            auto *comp2 = E2->getComponent<CollisionComponent>();
            auto v1 = comp1->vertices;
            auto v2 = comp2->vertices;
            for (auto &v : v1)
            {
                v = E1->getLocalToWorldMatrix() * glm::vec4(v, 1);
            }
            for (auto &v : v2)
            {
                v = E2->getLocalToWorldMatrix() * glm::vec4(v, 1);
            }

            // Compute the edge vectors for each box
            std::vector<glm::vec3> edges1;
            for (size_t i = 0; i < v1.size(); i++)
            {
                glm::vec3 e = v1[(i + 1) % v1.size()] - v1[i];
                edges1.push_back(glm::normalize(glm::cross(e, v1[(i + 2) % v1.size()] - v1[i])));
            }
            std::vector<glm::vec3> edges2;
            for (size_t i = 0; i < v2.size(); i++)
            {
                glm::vec3 e = v2[(i + 1) % v2.size()] - v2[i];
                edges2.push_back(glm::normalize(glm::cross(e, v2[(i + 2) % v2.size()] - v2[i])));
            }

            // Compute the face normals for each box
            std::vector<glm::vec3> faceNormals1;
            for (size_t i = 0; i < v1.size(); i++)
            {
                glm::vec3 e1 = v1[(i + 1) % v1.size()] - v1[i];
                glm::vec3 e2 = v1[(i + 2) % v1.size()] - v1[(i + 1) % v1.size()];
                faceNormals1.push_back(glm::normalize(glm::cross(e1, e2)));
            }
            std::vector<glm::vec3> FaceNormals2;
            for (size_t i = 0; i < v2.size(); i++)
            {
                glm::vec3 e1 = v2[(i + 1) % v2.size()] - v2[i];
                glm::vec3 e2 = v2[(i + 2) % v2.size()] - v2[(i + 1) % v2.size()];
                FaceNormals2.push_back(glm::normalize(glm::cross(e1, e2)));
            }

            // Compute the cross products of all pairs of edge vectors and face normals
            std::vector<glm::vec3> axes;
            //            for (glm::vec3 e: edges1) {
            //                for (glm::vec3 n: FaceNormals2) {
            //                    axes.push_back(glm::normalize(glm::cross(e, n)));
            //                }
            //            }
            //            for (glm::vec3 e: edges2) {
            //                for (glm::vec3 n: faceNormals1) {
            //                    axes.push_back(glm::normalize(glm::cross(e, n)));
            //                }
            //            }

            for (glm::vec3 e : edges1)
            {
                //                axes.push_back(e);
                for (glm::vec3 e2 : edges2)
                {
                    axes.push_back(glm::normalize(glm::cross(e, e2)));
                }
            }
            //            for (glm::vec3 e: edges2) {
            //                axes.push_back(e);
            //            }
            for (glm::vec3 n : faceNormals1)
            {
                axes.push_back(n);
            }
            for (glm::vec3 n : FaceNormals2)
            {
                axes.push_back(n);
            }

            // Project the vertices of the two boxes onto each axis and find the minimum and maximum projections
            for (glm::vec3 axis : axes)
            {
                float min1 = glm::dot(v1[0], axis);
                float max1 = min1;
                float min2 = glm::dot(v2[0], axis);
                float max2 = min2;
                for (size_t i = 1; i < v1.size(); i++)
                {
                    float projection = glm::dot(v1[i], axis);
                    min1 = std::min(min1, projection);
                    max1 = std::max(max1, projection);
                }
                for (size_t i = 1; i < v2.size(); i++)
                {
                    float projection = glm::dot(v2[i], axis);
                    min2 = std::min(min2, projection);
                    max2 = std::max(max2, projection);
                }
                if (max1 < min2 || max2 < min1)
                {
                    return false; // There is a gap between the projections, so the boxes do not intersect
                }
            }

            // If there is no gap between the projections along any of the axes, then the boxes intersect
            return true;
        }

        int cc = 0;

        // This function is called every frame by the world to determine if there is any collision
        CollisionType update(World *world, float)
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
                if (entity->name == "player")
                {
                    stick_index = entity;
                }
            }

            if (!stick_index)
            {
                CollisionType::NONE;
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
                if (SatCollide(entity1, entity2))
                {
                    // if the two entities are colliding, then change delete the entity
                    CollisionType type = entity2->getComponent<CollisionComponent>()->type;
                    switch (type)
                    {
                    case CollisionType::COIN:
                        coins_collected++;
                        //                            std::cout << coins_collected << std::endl;
                        // if the coin is collided, marke it as collided
                        // so that the coin system will not redraw it again
                        // (to avoid confilict of both classes
                        // (i.e., we don't want both classes to modify the position of the same coin at
                        // the same time))
                        entity2->getComponent<CoinComponent>()->collided = true;
                        // then we will redraw it instead of deleting from the system
                        // cuase our game is infinite
                        // i.e., we need to redraw the coins
                        // otherwise, if we delete each collided coin, then if the player collide all coins
                        // there will be no coin again to be collected
                        entity2->getComponent<CoinComponent>()->getOwner()->localTransform.position.z -= 50;
                        return CollisionType::COIN;
                    case CollisionType::OBSTACLE:
                        lives--;
                        if (lives == 0)
                        {
                            is_lost = true;
                        }
                        entity2->getComponent<ObstacleComponent>()->collided = true;
                        entity2->getComponent<ObstacleComponent>()->getOwner()->localTransform.position.z -= 50;
                        // world->markForRemoval(entity2);
                        //                            is_lost = true;
                        //                            std::cout << "lost" << cc++ << std::endl;
                        return CollisionType::OBSTACLE;
                    case CollisionType::MONKEY:
                        lives--;
                        if (lives == 0)
                        {
                            is_lost = true;
                        }
                        entity2->getComponent<MonkeyComponent>()->collided = true;
                        entity2->getComponent<MonkeyComponent>()->getOwner()->localTransform.position.z -= 50;
                        // world->markForRemoval(entity2);
                        //                            is_lost = true;
                        //                            std::cout << "lost" << cc++ << std::endl;
                        return CollisionType::MONKEY;
                    default:
                        return CollisionType::NONE;
                    }
                }
            }
            CollisionType::OBSTACLE;
        }
    };

}
