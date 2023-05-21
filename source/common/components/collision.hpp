#pragma once

#include <iostream>
#include "../ecs/component.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/glm.hpp"
#include "../deserialize-utils.hpp"
#include "glm/gtc/matrix_transform.hpp"

enum class CollisionType
{
    NONE,
    COIN,
    OBSTACLE,
    MONKEY,
    CUBE
};

namespace our
{
    inline const std::unordered_map<std::string, CollisionType> collisionMap = {
        {"none", CollisionType::NONE},
        {"coin", CollisionType::COIN},
        {"obstacle", CollisionType::OBSTACLE},
        {"monkey", CollisionType::MONKEY},
        {"cube", CollisionType::CUBE}};

    class CollisionComponent : public Component
    {
    public:
        // the type of the collision
        // Can be NONE, COIN, OBSTACLE
        CollisionType type = CollisionType::NONE;

        // the vertices of the collision box
        // the vertices are in the order of the faces of the box so that
        // i can claculate the edges and the normals of the faces easily later
        // in the collision system
        std::vector<glm::vec3> vertices;

        static std::string getID() { return "Collision"; }

        // Reads linearVelocity & angularVelocity from the given json object
        void deserialize(const nlohmann::json &data) override
        {
            if (!data.is_object())
                return;
            // extension in X direction
            glm::vec2 W = data.value("W", glm::vec2(1.0f, 1.0f));
            // extension in Y direction
            glm::vec2 H = data.value("H", glm::vec2(1.0f, 1.0f));
            // extension in Z direction
            glm::vec2 D = data.value("D", glm::vec2(1.0f, 1.0f));

            // add vertices to the vector
            vertices.emplace_back(W.y, H.y, D.y);
            vertices.emplace_back(W.y, H.y, D.x);
            vertices.emplace_back(W.y, H.x, D.x);
            vertices.emplace_back(W.y, H.x, D.y);
            vertices.emplace_back(W.x, H.x, D.y);
            vertices.emplace_back(W.x, H.x, D.x);
            vertices.emplace_back(W.x, H.y, D.x);
            vertices.emplace_back(W.x, H.y, D.y);

            // get the type of the collision
            std::string typeString = data.value("objType", "none");

            try
            {
                type = collisionMap.at(typeString);
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << '\n';
                std::cerr << "Collision type might not be correct: " << typeString << '\n';
                type = CollisionType::NONE;
            }
        };
    };

}