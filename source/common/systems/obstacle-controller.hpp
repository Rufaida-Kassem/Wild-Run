#pragma once

#include "../ecs/world.hpp"
#include "../components/obstacle.hpp"
#include "../components/free-camera-controller.hpp"

#include "../application.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <vector>


namespace our
{

	// the obstacle controller system is responsible for redrawing the uncollided obstacles so that 
	// during the game, there will be obstacles all the time and won't finish (disappear)
	// i.e., if this system doesn't exist, then the obstacles will appear only at the beg. of the game
	// for specific time until the user pass through the last obstacle
	// hence, no obstacles agian will exist for the rest of the game
	// Again, this is because our game is infinite
	class ObstacleControllerSystem {


	public:

		// This should be called every frame to update all entities containing a FreeCameraControllerComponent 
		void update(World* world, float deltaTime) {
			// search for the obstacles
			ObstacleComponent* obstacle = nullptr;
			// make an array of obstacles
			std::vector<ObstacleComponent*> obstacles;
			FreeCameraControllerComponent* controller = nullptr;
			// loop over all entities in the world 
			// and search for all obstacles and store them in a vector
			// also get the free camera controller component as we will need its position bellow
			for (auto entity : world->getEntities()) {
				obstacle = entity->getComponent<ObstacleComponent>();
				if(!controller)
					controller = entity->getComponent<FreeCameraControllerComponent>();
				if (obstacle) {
					obstacles.push_back(obstacle);
				}
			}
			
			// if there is no obstacles, then return
			if (obstacles.size() == 0) {
				return;
			}

			// if there is no controller, then return
			if (!controller) {
				return;
			}

			// get the position of the controller
			glm::vec3& camera_position = controller->getOwner()->localTransform.position;

			// loop over the obstacles and check if the camera is in front of the obstacle
			// if so then make the obstacle in front of the camera
			// i.e., let its z equals to the camera z - a length greater than the legnth
			// of the whole scene at the beg. of the game
			// I mean we want to avoid overlapping between all entities in the scene
			// we will check if the obstacle is behind the camera every frame
			for(auto obstacle : obstacles){
				// get the position of the obstacle
				glm::vec3& obstacle_position = obstacle->getOwner()->localTransform.position;
				// check if the obstacle is behind the camera
				if (obstacle_position.z > camera_position.z && obstacle->collided == false) {
					// make the obstacle in front of the camera
					// we will let this obstacle in front of the farest obstacle / coin (i.e, the farest entity)
					obstacle_position.z = obstacle_position.z - 50;
				}
				if(obstacle->collided == true){
					obstacle->collided = false;
				}
			}


		}

	};

}
