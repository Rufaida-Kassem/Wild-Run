#pragma once

#include <application.hpp>
#include <ecs/entity.hpp>
#include <ecs/world.hpp>
#include <systems/forward-renderer.hpp>
#include <systems/free-camera-controller.hpp>
#include <systems/movement.hpp>
#include <systems/collision.hpp>
#include <systems/coin-controller.hpp>
#include <systems/lightpole-position.hpp>
// incllude the road movement controller
#include <systems/road-movement-controller.hpp>
#include <systems/obstacle-controller.hpp>
#include <asset-loader.hpp>
#include <components/collision.hpp>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
// #include <irrKlang.h>
// #pragma comment(libs, "IrrKlang.libs")
// using namespace irrklang;

// This state shows how to use the ECS framework and deserialization.
class Playstate : public our::State {

    our::World world;
    our::ForwardRenderer renderer;
    our::FreeCameraControllerSystem cameraController;
    our::MovementSystem movementSystem;
    our::CollisionSystem collisionSystem;
    our::RoadControllerSystem roadController;
    our::CoinControllerSystem coinController;
    our::ObstacleControllerSystem obstacleController;
    our::LightPoleControllerSystem lightpoleController;
    our::PreviewCameraControllerSystem previewController;

    // ISoundEngine *SoundEngine = createIrrKlangDevice(); // = createIrrKlangDevice();

    void onInitialize() override {
        // SoundEngine->play2D("assets/sounds/theme.wav", true);
        //  the following line gives an error
        //  sndPlaySound("assets/sounds/theme.wav",SND_ASYNC);
        //  First of all, we get the scene configuration from the app config
        auto &config = getApp()->getConfig()["scene"];
        // If we have assets in the scene config, we deserialize them
        if (config.contains("assets")) {
            our::deserializeAllAssets(config["assets"]);
        }
        // If we have a world in the scene config, we use it to populate our world
        if (config.contains("world")) {
            world.deserialize(config["world"]);
        }
        // We initialize the camera controller system since it needs a pointer to the app
        cameraController.enter(getApp());
        // Then we initialize the renderer
        auto size = getApp()->getFrameBufferSize();
        renderer.initialize(size, config["renderer"]);
        collisionSystem.OnInitialize();
        previewController.enter(getApp(), &world);
        previewController.deserializePlayers(config["players-entities"]);

//        SoundEngine->play2D("assets/sounds/theme.wav", true);
    }

    void onDraw(double deltaTime) override {
        // Here, we just run a bunch of systems to control the world logic
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        movementSystem.update(&world, (float) deltaTime);
        cameraController.update(&world, (float) deltaTime);
        // TODO: update the road movement controller
        roadController.update(&world, (float) deltaTime);
        coinController.update(&world, (float) deltaTime);
        obstacleController.update(&world, (float) deltaTime);
        lightpoleController.update(&world, (float) deltaTime);

        collisionSystem.update(&world, (float) deltaTime);
        world.deleteMarkedEntities();
        // And finally we use the renderer system to draw the scene
        renderer.render(&world);

        // Get a reference to the keyboard object
        auto &keyboard = getApp()->getKeyboard();
        // If the escape key is pressed, go to the menu state
        if (keyboard.justPressed(GLFW_KEY_ESCAPE)) {
            // If the escape  key is pressed in this frame, go to the play state
            getApp()->changeState("menu");
        }
        // If the player is lost, go to the game over state
        if (collisionSystem.get_is_lost()) {

            getApp()->changeState("game-over");
        }
    }

    void onImmediateGui() override {
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
        // resize the window
        //        ImGui::SetWindowSize(ImVec2(300, 100));
        //            get the size of the text
        //        ImVec2 text_size = ImGui::CalcTextSize(current_coins.c_str());
        ImGui::Text(current_coins.c_str());
        ImGui::Text(current_lives.c_str());
        ImGui::End();
    }

    void onDestroy() override {
        // destroy the obstacle controller
//        SoundEngine->setAllSoundsPaused();
        obstacleController.cleanUp();
        // destroy the coin controller
        coinController.cleanUp();
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