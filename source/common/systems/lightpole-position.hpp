#pragma once

#include "../ecs/world.hpp"
#include "../components/lightpole.hpp"
#include "../components/free-camera-controller.hpp"

#include "../application.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <vector>


namespace our
{

	// the lightpole controller system is responsible for redrawing the lightpoles so that 
	// during the game, there will be lightpoles all the time and won't finish (disappear)
	// i.e., if this system class doesn't exist, then the lightpoles will appear only at the beg. of the game
	// for specific time until the user pass through the last lightpole
	// hence, no lightpoles agian will exist for the rest of the game
	// Again, this is because our game is infinite
	class LightPoleControllerSystem {


	public:
        // make an array of lightpoles
		std::vector<LightPoleComponent*> lightpoles;
		FreeCameraControllerComponent* controller = nullptr;

		// This should be called every frame to update all entities containing a FreeCameraControllerComponent 
		void update(World* world, float deltaTime) {
			// search for the lightpoles
			LightPoleComponent* lightpole = nullptr;
			// loop over all entities in the world 
			// and search for all lightpoles and store them in a vector
			// also get the free camera controller component as we will need its position bellow
			for (auto entity : world->getEntities()) {
				lightpole = entity->getComponent<LightPoleComponent>();
				if(!controller)
					controller = entity->getComponent<FreeCameraControllerComponent>();
				if (lightpole) {
					lightpoles.push_back(lightpole);
				}
			}
			
			// if there is no lightpoles or controller, then return
			if (lightpoles.size() == 0 || !controller) {
				return;
			}


			// get the position of the controller
			glm::vec3& camera_position = controller->getOwner()->localTransform.position;

			// loop over the lightpoles and check if the camera is in front of the lightpole
			// if so then make the lightpole in front of the camera
			// i.e., let its z equals to the camera z - a length greater than the legnth
			// of the whole scene at the beg. of the game
			// I mean we want to avoid overlapping between all entities in the scene
			// we will check if the lightpole is behind the camera every frame
			for(auto lightpole : lightpoles){
				// get the position of the lightpole
				glm::vec3& lightpole_position = lightpole->getOwner()->localTransform.position;
				// check if the lightpole is behind the camera
				if (lightpole_position.z > camera_position.z) {
					// make the lightpole in front of the camera
					// we will let this lightpole in front of the farest lightpole / coin (i.e, the farest entity)
					lightpole_position.z = lightpole_position.z - 50;
				}

			}


		}

        // clean the lightpoles vector and let the controller pointer points to null
        void cleanUp(){
            lightpoles.clear();
            controller = nullptr; // if we delete this line then the game will crash cuz we will try to access a null pointer in the update function above but after adding this line, the game will not crash cuz we will not access the null pointer anymore as we will not enter the update function anymore as the lightpoles vector is empty
        }
	};

}
