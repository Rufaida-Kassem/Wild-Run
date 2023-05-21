#pragma once

#include "../ecs/world.hpp"
#include "../components/monkey.hpp"
#include "../components/free-camera-controller.hpp"

#include "../application.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <vector>

namespace our {

    class MonkeyControllerSystem {

    public:
        MonkeyComponent *monkey = nullptr;
        // make array of monkeys
        std::vector<MonkeyComponent *> monkeys;
        FreeCameraControllerComponent *controller = nullptr;

        // This should be called every frame to update all entities containing a FreeCameraControllerComponent
        void update(World *world, float deltaTime) {
            // search for the monkeys
            // loop around all monkeys in the world
            // collect them in a vector
            // also pick the camera controller as we need its z position below
            for (auto entity: world->getEntities()) {
                monkey = entity->getComponent<MonkeyComponent>();
                if (!controller)
                    controller = entity->getComponent<FreeCameraControllerComponent>();
                if (monkey) {
                    monkeys.push_back(monkey);
                }
            }

            // if there is no monkeys, then return
            if (monkeys.size() == 0) {
                return;
            }

            // if there is no controller, then return
            if (!controller) {
                return;
            }

            // get the position of the controller
            glm::vec3 &camera_position = controller->getOwner()->localTransform.position;

            // loop over the monkeys and check if the camera is in front of the monkey
            // if so then make the monkey in front of the camera
            // i.e., let its z equals to the camera z - a position which is greater tha the last monkey --> minus cause we are moving in the -ve z
            // 50 is satisfied so that no 2 monkeys will be drawn on each other
            // we will check if the monkey is behind the camera every frame
            for (auto monkey: monkeys) {
                // get the position of the monkey
                glm::vec3 &monkey_position = monkey->getOwner()->localTransform.position;
                // check if the monkey is behind the camera
                // In addition to that we will need to check if the monkey is collided,
                // if so then no need to update its position cause the collision system will do that
                // instead of deleting this monkey
                if (monkey != nullptr && monkey_position.z > camera_position.z && monkey->collided == false) {
                    // make the monkey in front of the camera
                    // we will let this monkey in front of the farest monkey
                    monkey_position.z = monkey_position.z - 50;
                }
                // if the collection system mark the monkey as collided so we need to return its state (collided or not) back again
                // as this monkey will be considered as a new monkey different from the collided monkey
                // i.e, it can be collided again
                if (monkey->collided == true) {
                    monkey->collided = false;
                }
            }
        }

        // clean up the coins vector
        void cleanUp() {
            monkeys.clear();
            controller = nullptr;
        }
    };

}
