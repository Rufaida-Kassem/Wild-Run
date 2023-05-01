#pragma once

#include <iostream>
#include "../ecs/component.hpp"
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
		float radius = 0.1f; // Each frame, the entity should move as follows: position += linearVelocity * deltaTime
		float width = 0.0;
		float height = 0.0;

		CollisionType type = CollisionType::NONE;
		static std::string getID() { return "Collision"; }

		// Reads linearVelocity & angularVelocity from the given json object
		void deserialize(const nlohmann::json& data) override
		{
			if (!data.is_object())
				return;
			radius = data.value("radius", radius);
			width = data.value("width", width);
			height = data.value("height", height);


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