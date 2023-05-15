#pragma once

#include <application.hpp>
#include <ecs/entity.hpp>
#include <ecs/world.hpp>
#include <systems/forward-renderer.hpp>
#include <systems/free-camera-controller.hpp>
#include <systems/movement.hpp>
#include <systems/collision.hpp>
#include <systems/coin-controller.hpp>
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
// #pragma comment(lib, "irrKlang.lib")
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
    //ISoundEngine *SoundEngine = createIrrKlangDevice();// = createIrrKlangDevice();

    void onInitialize() override {
        //SoundEngine->play2D("assets/sounds/theme.wav", true);
        // the following line gives an error 
        // sndPlaySound("assets/sounds/theme.wav",SND_ASYNC);
        // First of all, we get the scene configuration from the app config
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
        
        
        collisionSystem.update(&world, (float) deltaTime);
        world.deleteMarkedEntities();
        // And finally we use the renderer system to draw the scene
        renderer.render(&world);

        // Get a reference to the keyboard object
        auto &keyboard = getApp()->getKeyboard();

        if (keyboard.justPressed(GLFW_KEY_ESCAPE)) {
            // If the escape  key is pressed in this frame, go to the play state
            getApp()->changeState("menu");
        }
        if (collisionSystem.get_is_lost()) {

            getApp()->changeState("game-over");
        }
    }

    void print_vec45(glm::vec4 v) {
        std::cout << v.x << " " << v.y << " " << v.z << " " << v.w << std::endl;
    }

    void onImmediateGui() override {
        // write the current state name in text box
        ImGuiWindowFlags window_flags = 0;
        window_flags |= ImGuiWindowFlags_NoDecoration |
                        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings |
                        ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav |
                        ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar;
        ImGui::Begin("Play State", nullptr, window_flags);
        const std::string current_coins = "Coins: " + std::to_string(collisionSystem.get_coins_collected());
        // resize the window
        // ImGui::SetWindowSize(ImVec2(100, 500));
        ImGui::Text(current_coins.c_str());
        
        // print the position of camera and the roads
        // but the following will give error because the road1 and road2 are not initialized
        if(roadController.controller && roadController.road1 && roadController.road2) {
            std::string camerapos = std::to_string(roadController.controller->getOwner()->localTransform.position.z);
            ImGui::Text(camerapos.c_str());
            std::string road1pos = std::to_string(roadController.road1->getOwner()->localTransform.position.z);
            ImGui::Text(road1pos.c_str());
            std::string road2pos = std::to_string(roadController.road2->getOwner()->localTransform.position.z);
            ImGui::Text(road2pos.c_str());
        }
        else{
            if(!roadController.controller)
                ImGui::Text("No controller");
            else
                ImGui::Text("controller exists");
            if(!roadController.road1)
                ImGui::Text("No road1");
            else
                ImGui::Text("road1 exists");
            if(!roadController.road2)
                ImGui::Text("No road2");
            else
                ImGui::Text("road2 exists");
        }

        //our::Entity *E1 = world.getEntitiesByName("stick");
        //our::Entity *E2 = world.getEntitiesByName("MOON");
        //our::CollisionComponent *comp1 = E1->getComponent<our::CollisionComponent>();
        //our::CollisionComponent *comp2 = E2->getComponent<our::CollisionComponent>();
        //glm::vec3 min1 = E1->getLocalToWorldMatrix() * glm::vec4(comp1->limit_min, 1);
        //glm::vec3 max1 = E1->getLocalToWorldMatrix() * glm::vec4(comp1->limit_max, 1);
        //glm::vec3 min2 = E2->getLocalToWorldMatrix() * glm::vec4(comp2->limit_min, 1);
        //glm::vec3 max2 = E2->getLocalToWorldMatrix() * glm::vec4(comp2->limit_max, 1);
        //ImGui::Text("stick");
        //ImGui::Text("min1: %f %f %f", min1.x, min1.y, min1.z);
        //ImGui::Text("max1: %f %f %f", max1.x, max1.y, max1.z);
        //ImGui::Text("MOON");
        //ImGui::Text("min2: %f %f %f", min2.x, min2.y, min2.z);
        //ImGui::Text("max2: %f %f %f", max2.x, max2.y, max2.z);
        //ImGui::Text("MNK1");
        //E2 = world.getEntitiesByName("MNK1");
        //comp2 = E2->getComponent<our::CollisionComponent>();
        //min2 = E2->getLocalToWorldMatrix() * glm::vec4(comp2->limit_min, 1);
        //max2 = E2->getLocalToWorldMatrix() * glm::vec4(comp2->limit_max, 1);
        //ImGui::Text("min2: %f %f %f", min2.x, min2.y, min2.z);
        //ImGui::Text("max2: %f %f %f", max2.x, max2.y, max2.z);
        //// distance to monkey 1
        //float dist = glm::distance(min1, min2);
        //ImGui::Text("distance: %f", dist);

        //ImGui::Text("MNK2");
        //comp2 = E2->getComponent<our::CollisionComponent>();
        //E2 = world.getEntitiesByName("MNK2");
        //min2 = E2->getLocalToWorldMatrix() * glm::vec4(comp2->limit_min, 1);
        //max2 = E2->getLocalToWorldMatrix() * glm::vec4(comp2->limit_max, 1);
        //ImGui::Text("min2: %f %f %f", min2.x, min2.y, min2.z);
        //ImGui::Text("max2: %f %f %f", max2.x, max2.y, max2.z);

        ImGui::End();
    }

    void onDestroy() override {
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