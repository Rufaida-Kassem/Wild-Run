#pragma once

#include "../ecs/world.hpp"
#include "../components/collision.hpp"
#include "../components/coin.hpp"
#include "../components/obstacle.hpp"

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
    // The Collision System is responsible for detecting collisions between entities
    // that have a CollisionComponent attached to them.
    // It also handels the logic in case of a collision.
    class CollisionSystem
    {
        // number of coins collected by the player
        // when it reaches 5, the player wins
        int coins_collected = 0;

        // number of lives the player has
        // when it reaches 0, the player loses
        int lives;

        // a boolean that indicates whether the player has lost or not
        bool is_lost = false;

    public:
        void OnInitialize()
        {
            is_lost = false;
            coins_collected = 0;
            lives = 3;
        }

        // get the is_lost boolean
        bool get_is_lost()
        {
            return is_lost;
        }

        // get the number of coins collected by the player
        int get_coins_collected()
        {
            return coins_collected;
        }

        // get the number of lives the player has
        int get_lives()
        {
            return lives;
        }

        // http://programmerart.weebly.com/separating-axis-theorem.html
        // to understand the algorithm
        // i did not copy the code from the website but i did use the idea of the algorithm to implement it in 3D
        static bool SatCollide(Entity *E1, Entity *E2)
        {
            // get the collision components of the two entities
            auto *comp1 = E1->getComponent<CollisionComponent>();
            auto *comp2 = E2->getComponent<CollisionComponent>();
            // get the vertices of the collision box on the two entities
            auto v1 = comp1->vertices;
            auto v2 = comp2->vertices;

            // transform the vertices to world space by multiplying them by the local to world matrix of the entity
            for (auto &v : v1)
            {
                v = E1->getLocalToWorldMatrix() * glm::vec4(v, 1);
            }
            for (auto &v : v2)
            {
                v = E2->getLocalToWorldMatrix() * glm::vec4(v, 1);
            }

            // Compute the edge vectors for each box
            // edges are the vectors between two consecutive vertices of the box
            std::vector<glm::vec3> edges1;
            for (size_t i = 0; i < v1.size(); i++)
            {
                glm::vec3 e = v1[(i + 1) % v1.size()] - v1[i];
                edges1.push_back(glm::normalize(e));
            }
            std::vector<glm::vec3> edges2;
            for (size_t i = 0; i < v2.size(); i++)
            {
                glm::vec3 e = v2[(i + 1) % v2.size()] - v2[i];
                edges2.push_back(glm::normalize(e));
            }

            // Compute the face normals for each box
            // face normals are the cross products of the vectors of the edges of the box
            std::vector<glm::vec3> faceNormals1;
            for (size_t i = 0; i < v1.size(); i++)
            {
                glm::vec3 e1 = edges1[i];
                glm::vec3 e2 = edges1[(i + 1) % edges1.size()];
                faceNormals1.push_back(glm::normalize(glm::cross(e1, e2)));
            }
            std::vector<glm::vec3> FaceNormals2;
            for (size_t i = 0; i < v2.size(); i++)
            {
                glm::vec3 e1 = edges2[i];
                glm::vec3 e2 = edges2[(i + 1) % edges2.size()];
                FaceNormals2.push_back(glm::normalize(glm::cross(e1, e2)));
            }

            // Compute the axes to project onto
            // axes will be used to detect collisions
            std::vector<glm::vec3> axes;
            for (glm::vec3 e : edges1)
            {
                for (glm::vec3 e2 : edges2)
                {
                    axes.push_back(glm::normalize(glm::cross(e, e2)));
                }
            }
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
                // find the minimum and maximum projections of the vertices of the two boxes
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
                // if there is a gap between the projections, then the boxes do not intersect
                if (max1 < min2 || max2 < min1)
                {
                    return false;
                }
            }

            // If there is no gap between the projections along any of the axes, then the boxes intersect
            return true;
        }

        // This function is called every frame by the world to determine if there is any collision
        void update(World *world, float)
        {
            // For each entity in the world
            std::vector<Entity *> entitiesToCollide;
            Entity *player_index = nullptr;
            // get all the entities that have a collision component
            // and add them to the vector of entities to collide
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
                    player_index = entity;
                }
            }
            // if the player is not found, then return
            if (!player_index)
            {
                return;
            }
            Entity *entity1 = player_index;
            // check if the stick is colliding with the any of the other entities
            for (auto &i : entitiesToCollide)
            {
                if (i == player_index)
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
                        break;
                    case CollisionType::OBSTACLE:
                        lives--;
                        if (lives == 0)
                        {
                            is_lost = true;
                        }
                        entity2->getComponent<ObstacleComponent>()->collided = true;
                        entity2->getComponent<ObstacleComponent>()->getOwner()->localTransform.position.z -= 50;
                        break;
                    default:
                        break;
                    }
                }
            }
        }
    };

}
