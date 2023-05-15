#pragma once

#include "../ecs/world.hpp"
#include "../components/road.hpp"
#include "../components/free-camera-controller.hpp"

#include "../application.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>


namespace our
{

	// the road controller system is responsible for moving the road
    // This system is added as a slightly complex example for how use the ECS framework to implement logic.
    // For more information, see "common/components/free-camera-controller.hpp"
	class RoadControllerSystem {


	public:
		RoadComponent* road1;
		RoadComponent* road2;
		FreeCameraControllerComponent* controller;
		// for the function update
		// we want to draw an infinite road
		// so we need to move the road that is behind the camera to the front of the other road
		// and we need to make sure that the road is always in front of the camera
		// so we need to check if the road is behind the camera
		// if so then make it in front of the seocond road
		// i.e., let its z equals to the second road z + its length
		// we will check if the road is behind the camera every frame
		// but should we check for the z of road to be greater or less than the camera z?
		// if we check for the z of road to be greater than the camera z
		// then the road will be in front of the camera
		// but if we check for the z of road to be less than the camera z
		// then the road will be behind the camera
		// so we will check for the z of road to be less than the camera z
		// but should we compare it by the camera z or the camera z - the length of the road?
		// if we compare it by the camera z
		// then the road will be behind the camera
		// but if we compare it by the camera z - the length of the road
		// then the road will be in front of the camera



		// This should be called every frame to update all entities containing a FreeCameraControllerComponent 
		void update(World* world, float deltaTime) {
			// First of all, we search for an entity containing both a CameraComponent and a FreeCameraControllerComponent
			// As soon as we find one, we break
			//RoadComponent* road1 = nullptr;
            //RoadComponent* road2 = nullptr;
			//FreeCameraControllerComponent* controller = nullptr;
			for (auto entity : world->getEntities()) {

				// check the entity by name
				if(entity->name == "road1")
					road1 = entity->getComponent<RoadComponent>();
				else if(entity->name == "road2")
					road2 = entity->getComponent<RoadComponent>();

				if(!controller)
					controller = entity->getComponent<FreeCameraControllerComponent>();
				if (road1 && road2 && controller) break;
			}
			// If there is no entity with both a CameraComponent and a FreeCameraControllerComponent, we can do nothing so we return
			if (!(road1 && road2 && controller)) return;
			// Get the entity that we found via getOwner of camera (we could use controller->getOwner())
			Entity* entity1 = road1->getOwner();
			Entity* entity2 = road2->getOwner();

            // get the camera position
            glm::vec3& camera_position = controller->getOwner()->localTransform.position;


			// We get a reference to the entity's position and rotation
			glm::vec3& position1 = entity1->localTransform.position;
			glm::vec3& position2 = entity2->localTransform.position;

			// check if the road z position is behind the camera
            // if so then make it in front of the seocond road
            // i.e., let its z equals to the second road z + its length
			if(position1.z < position2.z)
			{
				if (position2 .z >= camera_position.z + 30) // 20 is the length of the road (scalling in y)
				{
					position2.z =  position1.z - 60;
				}
			}
			else{
				if (position1 .z >= camera_position.z + 30) // 20 is the length of the road (scalling in y)
				{
					position1.z =  position2.z - 60;
				}
			}
            


		

		}

	};

}
