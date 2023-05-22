#pragma once

#include <application.hpp>
#include <ecs/entity.hpp>
#include <ecs/world.hpp>
#include <systems/forward-renderer.hpp>
#include <systems/free-camera-controller.hpp>
#include <systems/movement.hpp>
#include <systems/collision.hpp>
#include <systems/coin-controller.hpp>
#include <systems/monkey-controller.hpp>
#include <systems/lightpole-position.hpp>
#include <systems/road-movement-controller.hpp>
#include <systems/obstacle-controller.hpp>
#include <systems/cube-controller.hpp>
#include <asset-loader.hpp>
#include <components/collision.hpp>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

// #include <irrKlang.h>
//  #pragma comment(libs, "IrrKlang.libs")
// using namespace irrklang;

// This state shows how to use the ECS framework and deserialization.
class Playstate : public our::State
{

    our::World world;
    our::ForwardRenderer renderer;
    our::FreeCameraControllerSystem cameraController;
    our::MovementSystem movementSystem;
    our::CollisionSystem collisionSystem;
    our::RoadControllerSystem roadController;
    our::CoinControllerSystem coinController;
    our::MonkeyControllerSystem monkeyController;
    our::CubeControllerSystem cubeController;
    our::ObstacleControllerSystem obstacleController;
    our::PreviewCameraControllerSystem previewController;
    our::LightPoleControllerSystem lightpoleController;

    // ISoundEngine *SoundEngine = createIrrKlangDevice();// = createIrrKlangDevice();

    // start: the moment when the post processing effect starts
    // time_diff: the time elapsed between the starting moment of post processing effect and now
    // effectDuration: the time after which the post processing effect is disabled
    clock_t start = 0;
    float time_diff = 0;
    int effectDuration = 100;

    void onInitialize() override
    {

        // SoundEngine->play2D("assets/sounds/theme.wav", true);
        //  the following line gives an error
        //  sndPlaySound("assets/sounds/theme.wav",SND_ASYNC);
        //  First of all, we get the scene configuration from the app config
        auto &config = getApp()->getConfig()["scene"];
        // If we have assets in the scene config, we deserialize them
        if (config.contains("assets"))
        {
            our::deserializeAllAssets(config["assets"]);
        }
        // If we have a world in the scene config, we use it to populate our world
        if (config.contains("world"))
        {
            world.deserialize(config["world"]);
        }
        // We initialize the camera controller system since it needs a pointer to the app
        cameraController.enter(getApp());
        // Then we initialize the renderer
        auto size = getApp()->getFrameBufferSize();
        renderer.initialize(size, config["renderer"]);
        // init the required systems
        collisionSystem.OnInitialize();
        previewController.enter(getApp(), &world);

        // make sure that the preview camera reads the players avatars from the config file
        previewController.deserializePlayers(config["players-entities"]);
        //        SoundEngine->play2D("assets/sounds/theme.wav", true);
    }

    void onDraw(double deltaTime) override
    {
        // Here, we just run a bunch of systems to control the world logic
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        world.deleteMarkedEntities();

        movementSystem.update(&world, (float)deltaTime);
        cameraController.update(&world, (float)deltaTime);
        // TODO: update the road movement controller
        roadController.update(&world, (float)deltaTime);
        coinController.update(&world, (float)deltaTime);
        monkeyController.update(&world, (float)deltaTime);
        obstacleController.update(&world, (float)deltaTime);
        cubeController.update(&world, (float)deltaTime);
        lightpoleController.update(&world, (float)deltaTime);
        // we must make sure that the needed entities to be deleted
        //  are deleted before the collision system
        world.deleteMarkedEntities();
        CollisionType CollidedObject = collisionSystem.update(&world, (float)deltaTime);

        // if the collided object is monkey then apply a post processing effect and add noise to the position to shake the screen
        // by store the moment of collision in start and the enable post processing effect and noise
        // note: make sure the time_diff = 0 to avoid accumlation while rendering frames
        if (CollidedObject == CollisionType::MONKEY)
        {

            start = clock();
            renderer.effect = true;
            time_diff = 0;
            cameraController.shake = true;
        }
        // if the collided object is cube then increase the speed of the player as it is a punishment
        if (CollidedObject == CollisionType::CUBE)
        {
            cameraController.punishment *= 1.5;
        }

        // check if the time of post processing effect is finished then disable it and noise as well
        // make start and time_diff = 0 (initial state) to be ready for another collition
        if (renderer.effect && time_diff >= effectDuration)
        {
            renderer.effect = false;
            start = 0;
            cameraController.shake = false;
            time_diff = 0;
        }
        // if no collition happen then do nothing
        // we calculate the time_diff here to make use of it it disabling the post processing effect and noise
        // so we need to make it equal zero in above if statement
        else
        {
            time_diff += float(clock() - start) / CLOCKS_PER_SEC;
        }
        // And finally we use the renderer system to draw the scene
        renderer.render(&world);
        // Get a reference to the keyboard object
        auto &keyboard = getApp()->getKeyboard();

        if (keyboard.justPressed(GLFW_KEY_ESCAPE))
        {
            // If the escape  key is pressed in this frame, go to the play state
            getApp()->changeState("menu");
        }
        // if player is lost then go to game-over
        if (collisionSystem.get_is_lost())
        {

            getApp()->changeState("game-over");
        }
    }

    void onImmediateGui() override
    {
        // write the current state name in text box
        ImGuiWindowFlags window_flags = 0;
        window_flags |= ImGuiWindowFlags_NoDecoration |
                        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings |
                        ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav |
                        ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar;
        ImGui::Begin("Play State", nullptr, window_flags);
        ImGui::SetWindowPos(ImVec2(0, 0));
        ImGui::SetWindowSize(ImVec2(300, 100));
        const std::string current_coins = "Coins: " + std::to_string(collisionSystem.get_coins_collected());
        const std::string current_lives = "Lives: " + std::to_string(collisionSystem.get_lives());
        ImGui::Text(current_coins.c_str());
        ImGui::Text(current_lives.c_str());
        ImGui::End();
    }

    void onDestroy() override
    {
        // destroy the obstacle controller
        obstacleController.cleanUp();
        // destroy the coin controller
        coinController.cleanUp();
        // destroy the monkey controller
        monkeyController.cleanUp();
        // destroy the cube controller
        cubeController.cleanUp();
        // destroy the light pole controller
        lightpoleController.cleanUp();
        // destroy the road controller
        roadController.cleanUp();
        // Don't forget to destroy the renderer
        renderer.destroy();
        // On exit, we call exit for the camera controller system to make sure that the mouse is unlocked
        cameraController.exit();
        // Clear the world
        world.clear();
        // and we delete all the loaded assets to free memory on the RAM and the VRAM
        our::clearAllAssets();
    }
};