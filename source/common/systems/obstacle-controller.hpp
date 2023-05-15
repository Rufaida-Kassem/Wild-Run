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


	class ObstacleControllerSystem {


	public:

		// This should be called every frame to update all entities containing a FreeCameraControllerComponent 
		void update(World* world, float deltaTime) {
			// search for the obstacles
			ObstacleComponent* obstacle = nullptr;
			// make array of obstacles
			std::vector<ObstacleComponent*> obstacles;
			FreeCameraControllerComponent* controller = nullptr;
			for (auto entity : world->getEntities()) {
				obstacle = entity->getComponent<ObstacleComponent>();
				if(!controller)
					controller = entity->getComponent<FreeCameraControllerComponent>();
				if (obstacle) {
					obstacles.push_back(obstacle);
				}
			}
			
			// if there is no obstacles
			if (obstacles.size() == 0) {
				return;
			}

			// if there is no controller
			if (!controller) {
				return;
			}

			// get the position of the controller
			glm::vec3& camera_position = controller->getOwner()->localTransform.position;

			// loop over the obstacles and check if the camera is in front of the obstacle
			// if so then make the obstacle in front of the camera
			// i.e., let its z equals to the camera z + its length
			// we will check if the obstacle is behind the camera every frame
			for(auto obstacle : obstacles){
				// get the position of the obstacle
				glm::vec3& obstacle_position = obstacle->getOwner()->localTransform.position;
				// check if the obstacle is behind the camera
				if (obstacle_position.z > camera_position.z) {
					// make the obstacle in front of the camera
					// we will let this obstacle in front of the farest obstacle
					obstacle_position.z = obstacle_position.z - 50;
				}
			}


		}

	};

}
