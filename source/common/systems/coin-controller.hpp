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
		CoinComponent* coin = nullptr;
		// make array of coins
		std::vector<CoinComponent*> coins;
		FreeCameraControllerComponent* controller = nullptr;



		// This should be called every frame to update all entities containing a FreeCameraControllerComponent 
		void update(World* world, float deltaTime) {
			// search for the coins
			// loop around all coins in the world
			// collect them in a vector
			// also pick the camera controller as we need its z position below
			for (auto entity : world->getEntities()) {
				coin = entity->getComponent<CoinComponent>();
				if(!controller)
					controller = entity->getComponent<FreeCameraControllerComponent>();
				if (coin) {
					coins.push_back(coin);
				}
			}
			
			// if there is no coins, then return
			if (coins.size() == 0) {
				return;
			}

			// if there is no controller, then return
			if (!controller) {
				return;
			}

			// get the position of the controller
			glm::vec3& camera_position = controller->getOwner()->localTransform.position;

			// loop over the coins and check if the camera is in front of the coin
			// if so then make the coin in front of the camera
			// i.e., let its z equals to the camera z - a position which is greater tha the last coin --> minus cause we are moving in the -ve z
			// 50 is satisfied so that no 2 coins will be drawn on each other
			// we will check if the coin is behind the camera every frame
			for(auto coin : coins){
				// get the position of the coin
				glm::vec3& coin_position = coin->getOwner()->localTransform.position;
				// check if the coin is behind the camera
				// In addition to that we will need to check if the coin is collided,
				// if so then no need to update its position cause the collision system will do that 
				// instead of deleting this coin 
				if (coin_position.z > camera_position.z && coin->collided == false) {
					// make the coin in front of the camera
					// we will let this coin in front of the farest coin
					coin_position.z = coin_position.z - 50;
					
				}
				// if the collection system mark the coin as collided so we need to return its state (collided or not) back again
				// as this coin will be considered as a new coin different from the collided coin
				// i.e, it can be collided again
				if(coin->collided == true){
					coin->collided = false;
				}
			}


		}

	};

}
