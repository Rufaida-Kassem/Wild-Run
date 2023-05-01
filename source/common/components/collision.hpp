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
	OBSTACLE
};

namespace our
{
	inline const std::unordered_map<std::string, CollisionType> collisionMap = {
		{"none", CollisionType::NONE},
		{"coin", CollisionType::COIN},
		{"obstacle", CollisionType::OBSTACLE} };

	class CollisionComponent : public Component
	{
	public:
		// cylinder collision with radiusx and radiusz and height y
		glm::vec3 limit_min = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 limit_max = glm::vec3(1.0f, 1.0f, 1.0f);
		CollisionType type = CollisionType::NONE;
		static std::string getID() { return "Collision"; }

		// Reads linearVelocity & angularVelocity from the given json object
		void deserialize(const nlohmann::json& data) override
		{
			if (!data.is_object())
				return;
			limit_min = data.value("limit_min", limit_min);
			limit_max = data.value("limit_max", limit_max);
			std::string typeString = data.value("objType", "none");

			try
			{
				type = collisionMap.at(typeString);
			}
			catch (const std::exception& e)
			{
				std::cerr << e.what() << '\n';
				std::cerr << "Collision type might not be correct: " << typeString << '\n';
			}
		};
	};

}