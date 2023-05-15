#pragma once

#include "../ecs/world.hpp"
#include "../components/coin.hpp"
#include "../components/free-camera-controller.hpp"

#include "../application.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <vector>


namespace our
{


	class CoinControllerSystem {


	public:

		// This should be called every frame to update all entities containing a FreeCameraControllerComponent 
		void update(World* world, float deltaTime) {
			// search for the coins
			CoinComponent* coin = nullptr;
			// make array of coins
			std::vector<CoinComponent*> coins;
			FreeCameraControllerComponent* controller = nullptr;
			for (auto entity : world->getEntities()) {
				coin = entity->getComponent<CoinComponent>();
				if(!controller)
					controller = entity->getComponent<FreeCameraControllerComponent>();
				if (coin) {
					coins.push_back(coin);
				}
			}
			
			// if there is no coins
			if (coins.size() == 0) {
				return;
			}

			// if there is no controller
			if (!controller) {
				return;
			}

			// get the position of the controller
			glm::vec3& camera_position = controller->getOwner()->localTransform.position;

			// loop over the coins and check if the camera is in front of the coin
			// if so then make the coin in front of the camera
			// i.e., let its z equals to the camera z + its length
			// we will check if the coin is behind the camera every frame
			for(auto coin : coins){
				// get the position of the coin
				glm::vec3& coin_position = coin->getOwner()->localTransform.position;
				// check if the coin is behind the camera
				if (coin_position.z > camera_position.z) {
					// make the coin in front of the camera
					// we will let this coin in front of the farest coin
					coin_position.z = coin_position.z - 50;
				}
			}


		}

	};

}
