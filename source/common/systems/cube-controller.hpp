#pragma once

#include "../ecs/world.hpp"
#include "../components/cube.hpp"
#include "../components/free-camera-controller.hpp"

#include "../application.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <vector>

namespace our
{

    class CubeControllerSystem
    {

    public:
        CubeComponent *cube = nullptr;
        // make array of cubes
        std::vector<CubeComponent *> cubes;
        FreeCameraControllerComponent *controller = nullptr;

        // This should be called every frame to update all entities containing a FreeCameraControllerComponent
        void update(World *world, float deltaTime)
        {
            // search for the cubes
            // loop around all cubes in the world
            // collect them in a vector
            // also pick the camera controller as we need its z position below
            for (auto entity : world->getEntities())
            {
                cube = entity->getComponent<CubeComponent>();
                if (!controller)
                    controller = entity->getComponent<FreeCameraControllerComponent>();
                if (cube)
                {
                    cubes.push_back(cube);
                }
            }

            // if there is no cubes, then return
            if (cubes.size() == 0)
            {
                return;
            }

            // if there is no controller, then return
            if (!controller)
            {
                return;
            }

            // get the position of the controller
            glm::vec3 &camera_position = controller->getOwner()->localTransform.position;

            // loop over the cubes and check if the camera is in front of the cube
            // if so then make the cube in front of the camera
            // i.e., let its z equals to the camera z - a position which is greater tha the last cube --> minus cause we are moving in the -ve z
            // 50 is satisfied so that no 2 cubes will be drawn on each other
            // we will check if the cube is behind the camera every frame
            for (auto cube : cubes)
            {
                // get the position of the cube
                glm::vec3 &cube_position = cube->getOwner()->localTransform.position;
                // check if the cube is behind the camera
                // In addition to that we will need to check if the cube is collided,
                // if so then no need to update its position cause the collision system will do that
                // instead of deleting this cube
                if (cube != nullptr && cube_position.z > camera_position.z && cube->collided == false)
                {
                    // make the cube in front of the camera
                    // we will let this cube in front of the farest cube
                    cube_position.z = cube_position.z - 50;
                }
                // if the collection system mark the cube as collided so we need to return its state (collided or not) back again
                // as this cube will be considered as a new cube different from the collided cube
                // i.e, it can be collided again
                if (cube->collided == true)
                {
                    cube->collided = false;
                }
            }
        }
    };

}
