#pragma once

#include "../ecs/world.hpp"
#include "../components/camera.hpp"
#include "../components/free-camera-controller.hpp"

#include "../application.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtx/fast_trigonometry.hpp>
#include <iostream>

namespace our
{

    // The free camera controller system is responsible for moving every entity which contains a FreeCameraControllerComponent.
    // This system is added as a slightly complex example for how use the ECS framework to implement logic.
    // For more information, see "common/components/free-camera-controller.hpp"
    class FreeCameraControllerSystem
    {
        Application *app;          // The application in which the state runs
        bool mouse_locked = false; // Is the mouse locked
        bool IsJump = false;
        bool GoingUp = false;

        bool GoingLeft = false;
        bool GoingRight = false;
        float DistanceBetweenTracks = 1.0f;
        float jumpDistance = 1.2f;
        clock_t start = 0;
        float time_diff = 0;
        int speedTime = 10000;

        enum Track
        {
            LEFT = -1,
            CENTER,
            RIGHT,
        };

        Track CurrentTrack = CENTER;

        void UpdateTrack(Track &Current, bool LeftButton)
        {
            switch (LeftButton)
            {
            // left button is pressed
            case 1:
                switch (Current)
                {
                case LEFT:
                    break;
                case RIGHT:
                    Current = CENTER;
                    break;
                case CENTER:
                    Current = LEFT;
                    break;
                }
                break;

                // right button is pressed
            case 0:
                switch (Current)
                {
                case LEFT:
                    Current = CENTER;
                    break;
                case RIGHT:
                    break;
                case CENTER:
                    Current = RIGHT;
                    break;
                }
                break;
            }
        }

    public:
        // Wefihen a state enters, it should call this function and give it the pointer to the application
        inline static float punishment = 1;
        inline static bool shake = false;
        void enter(Application *app)
        {
            this->app = app;
        }

        // This should be called every frame to update all entities containing a FreeCameraControllerComponent
        void update(World *world, float deltaTime)
        {
            // First of all, we search for an entity containing a CameraComponent and another one conataining a FreeCameraControllerComponent
            // As soon as we find one, we break
            CameraComponent *camera = nullptr;
            FreeCameraControllerComponent *controller = nullptr;
            for (auto entity : world->getEntities())
            {
                controller = entity->getComponent<FreeCameraControllerComponent>();
                if (controller)
                    break;
            }

            for (auto entity : world->getEntities())
            {
                camera = entity->getComponent<CameraComponent>();
                if (camera)
                    break;
            }
            // Get the entity that we found via getOwner of this entity
            Entity *player = controller->getOwner();
            Entity *World = camera->getOwner();

            // If the left mouse button is pressed, we lock and hide the mouse. This common in First Person Games.
            if (app->getMouse().isPressed(GLFW_MOUSE_BUTTON_1) && !mouse_locked)
            {
                app->getMouse().lockMouse(app->getWindow());
                mouse_locked = true;
                // If the left mouse button is released, we unlock and unhide the mouse.
            }
            else if (!app->getMouse().isPressed(GLFW_MOUSE_BUTTON_1) && mouse_locked)
            {
                app->getMouse().unlockMouse(app->getWindow());
                mouse_locked = false;
            }

            // We get a reference to the entity's position
            glm::vec3 &position = player->localTransform.position;
            glm::vec3 &WorldPosition = World->localTransform.position;

            // We get the camera model matrix (relative to its parent) to compute the front, up and right directions
            glm::mat4 matrix = player->localTransform.toMat4();
            glm::vec3 front = glm::vec3(matrix * glm::vec4(0, 0, -1, 0)),
                      up = glm::vec3(matrix * glm::vec4(0, 1, 0, 0)),
                      right = glm::vec3(matrix * glm::vec4(1, 0, 0, 0));

            glm::vec3 current_sensitivity = controller->positionSensitivity;
            current_sensitivity *= punishment;
            // If the LEFT SHIFT key is pressed, we multiply the position sensitivity by the speed up factor
            if (app->getKeyboard().isPressed(GLFW_KEY_LEFT_SHIFT))
                current_sensitivity *= controller->speedupFactor;

            // to continuously move the world and the player forward
            position += front * (deltaTime * current_sensitivity.z);
            WorldPosition += front * (deltaTime * current_sensitivity.z);

            // left arrow moves the player to the left track
            if (app->getKeyboard().justPressed(GLFW_KEY_LEFT) && !GoingRight)
            {
                GoingLeft = true;
                UpdateTrack(CurrentTrack, 1);
            }
            if (GoingLeft)
            {
                position -= right * (deltaTime * current_sensitivity.x);
                if (position.x < CurrentTrack * DistanceBetweenTracks)
                {
                    GoingLeft = false;
                    position.x = CurrentTrack * DistanceBetweenTracks;
                }
            }
            // right arrow moves the player to the right track
            if (app->getKeyboard().justPressed(GLFW_KEY_RIGHT) && !GoingLeft)
            {
                GoingRight = true;
                UpdateTrack(CurrentTrack, 0);
            }
            if (GoingRight)
            {
                position += right * (deltaTime * current_sensitivity.x);
                if (position.x > CurrentTrack * DistanceBetweenTracks)
                {
                    GoingRight = false;
                    position.x = CurrentTrack * DistanceBetweenTracks;
                }
            }

            // space key makes the player jump
            if (app->getKeyboard().justPressed(GLFW_KEY_SPACE) && !IsJump)
            {
                IsJump = true;
                GoingUp = true;
            }
            if (IsJump)
            {
                if (GoingUp)
                {
                    position += up * (deltaTime * current_sensitivity.y);
                    if (position.y > jumpDistance)
                        GoingUp = false;
                }
                else
                {
                    position -= up * (deltaTime * current_sensitivity.y);
                    if (position.y < 0)
                    {
                        GoingUp = false;
                        IsJump = false;
                        position.y = 0;
                    }
                }
            }
            time_diff += float(clock() - start) / CLOCKS_PER_SEC;
            if (time_diff > speedTime)
            {
                time_diff = 0;
                start = clock();
                punishment *= 1.5;
            }

            if (shake)
            {
                position -= right * float((rand() % (1 - (-1) + 1)) + (-1)) / 20.0f;
            }
        }

        // When the state exits, it should call this function to ensure the mouse is unlocked
        void exit()
        {
            IsJump = false;
            if (mouse_locked)
            {
                mouse_locked = false;
                app->getMouse().unlockMouse(app->getWindow());
            }
        }
    };

}
