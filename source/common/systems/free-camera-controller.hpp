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

        // the following to booleans control jump operation
        // When IsJump is true, the player enters jump state and remain going up as long as GoingUp is true
        // when GoingUp is false that means that the player return to the road then return all controls variabls to initial state
        // so IsJump will be false and GoingUp will be false
        bool IsJump = false;
        bool GoingUp = false;
        // determine how high the player jumps
        float DistanceBetweenTracks = 1.0f;

        // the following booleans determine whethere the player is going to the left of the road or to right depending on which
        // boolean is true
        bool GoingLeft = false;
        bool GoingRight = false;
        // controls the distance between the tracks if it is to large the player will go out of the road
        // |       |       |
        // |--1.0--|--1.0--|
        // |       |       |
        float jumpDistance = 1.2f;

        // are used to increase the speed of the game over deterministic periods of time determined by speedTime
        clock_t start = 0;
        float time_diff = 0;
        int speedTime = 10000;

        // an enum deviding the road into tracks Left-center-right
        // intialize left with -1 to be used below to change the position to the left or to the right as
        // position.x = CurrentTrack * DistanceBetweenTracks
        // so LEFT has different sign from RIGHT to change the track
        enum Track
        {
            LEFT = -1,
            CENTER,
            RIGHT,
        };

        // inital state of track that player on it
        Track CurrentTrack = CENTER;

        // private function to update the current track used by the player depending on
        // which track the player is on it and which button is pressed
        void UpdateTrack(Track &Current, bool LeftButton)
        {
            switch (LeftButton)
            {
            // left button is pressed
            case 1:
                switch (Current)
                {
                    // the current track is left and left arrow is pressed so do nothing and break
                case LEFT:
                    break;
                    // the current track is right and left arrow is pressed so set the current track to be center
                case RIGHT:
                    Current = CENTER;
                    break;
                    // the current track is center and left arrow is pressed so set the current track to be left
                case CENTER:
                    Current = LEFT;
                    break;
                }
                break;

                // right button is pressed
            case 0:
                switch (Current)
                {
                    // the current track is left and right arrow is pressed so set the current track to be center
                case LEFT:
                    Current = CENTER;
                    break;
                    // the current track is right and right arrow is pressed so do nothing and break
                case RIGHT:
                    break;
                    // the current track is center and right arrow is pressed so set the current track to be right
                case CENTER:
                    Current = RIGHT;
                    break;
                }
                break;
            }
        }

    public:
        // static booleans
        // punishment: control the speed of the player
        // shake: shake the player as long as it is true
        inline static float punishment = 1;
        inline static bool shake = false;

        // define a state enters, it should call this function and give it the pointer to the application
        void enter(Application *app)
        {
            this->app = app;
            // reset punishment so that when you leave the game state and re-enter it again you move with the initial state of speed
            punishment = 1;
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

            // get the movement speed of player
            glm::vec3 current_sensitivity = controller->positionSensitivity;
            // apply punishment if exists
            current_sensitivity *= punishment;

            // If the LEFT SHIFT key is pressed, we multiply the position sensitivity by the speed up factor
            if (app->getKeyboard().isPressed(GLFW_KEY_LEFT_SHIFT))
                current_sensitivity *= controller->speedupFactor;

            // to continuously move the world and the player forward
            position += front * (deltaTime * current_sensitivity.z);
            WorldPosition += front * (deltaTime * current_sensitivity.z);

            // left arrow moves the player to the left track
            // set GoingLeft to be true and update the current track
            // we enter the scope of if condition whenever left arrow is pressed and GoingRight is false
            // so we cannot going right while we are going left
            if (app->getKeyboard().justPressed(GLFW_KEY_LEFT) && !GoingRight)
            {
                GoingLeft = true;
                UpdateTrack(CurrentTrack, 1);
            }

            // the logic of going left
            if (GoingLeft)
            {
                // move the player to the left and check if it reack to some point where it is further from distance between tracks
                // set going left to be false and position the player to the left
                position -= right * (deltaTime * current_sensitivity.x);
                if (position.x < CurrentTrack * DistanceBetweenTracks)
                {
                    GoingLeft = false;
                    position.x = CurrentTrack * DistanceBetweenTracks;
                }
            }

            // right arrow moves the player to the right track
            // set GoingRight to be true and update the current track
            // we enter the scope of if condition whenever right arrow is pressed and GoingLeft is false
            // so we cannot going left while we are going right
            if (app->getKeyboard().justPressed(GLFW_KEY_RIGHT) && !GoingLeft)
            {
                GoingRight = true;
                UpdateTrack(CurrentTrack, 0);
            }
            // the logic of going right
            if (GoingRight)
            {
                // move the player to the right and check if it reack to some point where it is further from distance between tracks
                // set GoingRight to be false and position the player to the right
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
                // set booleans those controls the jumping operation to be true
                IsJump = true;
                GoingUp = true;
            }
            // the jump logic
            if (IsJump)
            {
                if (GoingUp)
                {
                    // here we are in the state of going up in jump so move the player up and check
                    // whenever the player reaches a certain point determined by jumpDistance
                    // set GoingUp to be false and the start to move donw the player
                    position += up * (deltaTime * current_sensitivity.y);
                    if (position.y > jumpDistance)
                        GoingUp = false;
                }
                else
                {
                    // here we are in the state of going down in jump so move the player down and check
                    // whenever the player reaches 0 set booleans to the initial state and the player to its initial state
                    position -= up * (deltaTime * current_sensitivity.y);
                    if (position.y < 0)
                    {
                        GoingUp = false;
                        IsJump = false;
                        position.y = 0;
                    }
                }
            }

            // calculate the time difference so that we increase speed every determenistic periods determined by speedTime
            time_diff += float(clock() - start) / CLOCKS_PER_SEC;
            if (time_diff > speedTime)
            {
                time_diff = 0;
                start = clock();
                punishment *= 1.5;
            }

            // if it is true so add noise represents in random values to the position
            if (shake)
            {
                position -= right * float((rand() % (1 - (-1) + 1)) + (-1)) / 20.0f;
            }
        }

        // When the state exits, it should call this function to ensure the mouse is unlocked
        void exit()
        {
            // set IsJump to be false so when we leave play state and and re enter it again
            // we starting on the road
            IsJump = false;
            if (mouse_locked)
            {
                mouse_locked = false;
                app->getMouse().unlockMouse(app->getWindow());
            }
        }
    };

}
