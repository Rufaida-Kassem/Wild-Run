#pragma once

#include "../ecs/world.hpp"
#include "../components/camera.hpp"
#include "../application.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

namespace our
{

    // The PreviewCameraControllerSystem is responsible for controlling a Preview section
    // of the game. It is responsible for moving the camera around the scene.
    class PreviewCameraControllerSystem
    {
        Application *app;          // The application in which the state runs
        bool mouse_locked = false; // Is the mouse locked
        World *world;
        // list of players models available
        std::vector<nlohmann::json> playersList;

    public:
        // index of the current player
        inline static int current_player = 0;

        // When a state enters, it should call this function and give it the pointer to the application
        void enter(Application *app, World *world)
        {
            this->app = app;
            this->world = world;
        }

        static int getCurrentPlayer()
        {
            return current_player;
        }

        int getPlayerCount()
        {
            return int(playersList.size());
        }

        // deserialize the players list from the given json object
        void deserializePlayers(const nlohmann::json &data)
        {
            playersList.clear();
            if (!data.is_array())
                return;
            /// loop around the data
            for (const auto &entityData : data)
            {
                playersList.push_back(entityData);
            }
            // change to first player
            changePlayer(current_player);
        }

        // change the current player to the one with the given index
        void changePlayer(int new_player)
        {
            // check if the index is valid
            if (new_player < 0 || new_player >= playersList.size())
            {
                return;
            }
            // remove old player to make space for the new one
            // the old and new player should have the same parent
            // so we save the parent and assign it to the new player
            auto E = world->getEntitiesByName("player");
            Entity *E_parent = nullptr;
            if (E != nullptr)
            {
                E_parent = E->parent;
                world->markForRemoval(E);
            }
            // add a new one with the same parent as the old one so that we maintain the same hierarchy
            auto newE = world->add();
            newE->deserialize(playersList[new_player]);
            newE->name = "player";
            newE->parent = E_parent;
            current_player = new_player;
        }

        // This should be called every frame to update all entities containing a PreviewCameraControllerComponent
        void update(float deltaTime)
        {
            // First of all, we search for an entity containing both a CameraComponent
            // As soon as we find one, we break
            CameraComponent *camera = nullptr;
            for (auto entity : world->getEntities())
            {
                camera = entity->getComponent<CameraComponent>();
                if (camera)
                    break;
            }
            // If there is no entity with a CameraComponent , we can do nothing, so we return
            if (!(camera))
                return;

            // If the left mouse button is pressed, we lock and hide the mouse
            if (app->getMouse().isPressed(GLFW_MOUSE_BUTTON_1) && !mouse_locked)
            {
                app->getMouse().lockMouse(app->getWindow());
                mouse_locked = true;
                // If the left mouse button is released, we unlock and un hide the mouse.
            }
            // if the mouse is released and it was locked, we unlock it
            else if (!app->getMouse().isPressed(GLFW_MOUSE_BUTTON_1) && mouse_locked)
            {
                app->getMouse().unlockMouse(app->getWindow());
                mouse_locked = false;
            }
            // get the player entity and the camera entity
            Entity *entity = world->getEntitiesByName("player");
            Entity *camera_entity = camera->getOwner();

            // get a entity rotation
            glm::vec3 &rotation = entity->localTransform.rotation;

            // If the left mouse button is pressed, we get the change in the mouse location
            // and use it to update the entity rotation
            if (app->getMouse().isPressed(GLFW_MOUSE_BUTTON_1))
            {
                glm::vec2 delta = app->getMouse().getMouseDelta();
                rotation.x += delta.y * 0.01f; // The y-axis controls the pitch
                rotation.y += delta.x * 0.01f; // The x-axis controls the yaw
            }

            // We prevent the pitch from exceeding a certain angle to prevent gimbal locks
            if (rotation.x < -glm::pi<float>() * 0.99f)
                rotation.x = -glm::pi<float>() * 0.99f;
            if (rotation.x > 0)
                rotation.x = 0;
            // This is not necessary, but whenever the rotation goes outside the 0 to 2*PI range, we wrap it back inside.
            // This could prevent floating point error if the player rotates in single direction for an extremely long time.
            rotation.y = glm::wrapAngle(rotation.y);

            // we get the ortho height and update it based on the mouse scroll offset
            // it gives a zoom in and out effect
            float orthoHeight = camera->orthoHeight + app->getMouse().getScrollOffset().y * 0.2f;
            // we clamp it so that it doesn't go too far away or too close
            orthoHeight = glm::clamp(orthoHeight, 1.0f, 100.0f);
            // then set the camera ortho height to the new value
            camera->orthoHeight = orthoHeight;

            auto &keyboard = app->getKeyboard();
            // if the left or right arrow keys are pressed, we change the current player
            if (keyboard.justPressed(GLFW_KEY_LEFT))
            {
                if (current_player > 0)
                    changePlayer(current_player - 1);
            }
            if (keyboard.justPressed(GLFW_KEY_RIGHT))
            {
                if (current_player < playersList.size() - 1)
                    changePlayer(current_player + 1);
            }
        }

        // When the state exits, it should call this function to ensure the mouse is unlocked
        void exit()
        {
            if (mouse_locked)
            {
                mouse_locked = false;
                app->getMouse().unlockMouse(app->getWindow());
            }
        }
    };

}
