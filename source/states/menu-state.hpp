#pragma once

#include <application.hpp>
#include <shader/shader.hpp>
#include <texture/texture2d.hpp>
#include <texture/texture-utils.hpp>
#include <material/material.hpp>
#include <mesh/mesh.hpp>

#include <functional>
#include <array>
#include "systems/forward-renderer.hpp"
#include "systems/movement.hpp"
#include "systems/preview-camera-controller.hpp"

// This state shows how to use some of the abstractions we created to make a menu.
class Menustate : public our::State
{

    float time;
    // to render a background image
    our::ForwardRenderer renderer;
    our::World world;
    // to move the objects
    // our::MovementSystem movementSystem;
    // to control the preview camera
    our::PreviewCameraControllerSystem previewController;

    void onInitialize() override
    {
        // First of all, we get the scene configuration from the app config
        auto &config = getApp()->getConfig()["menu-scene"];
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
        // Then we initialize the renderer
        auto size = getApp()->getFrameBufferSize();
        // We initialize the renderer with a size of 3/4 of the window size height
        renderer.initialize({size.x, size.y * 3 / 4}, config["renderer"]);
        // We initialize the preview controller system
        previewController.enter(getApp(), &world);
        // We deserialize the players models from the config
        previewController.deserializePlayers(config["players-entities"]);
    }

    void onDraw(double deltaTime) override
    {
        // call the update function of the preview controller and the movement system to update the world
        previewController.update((float)deltaTime);
        // movementSystem.update(&world, (float)deltaTime);
        // Delete all the entities that are marked for deletion
        world.deleteMarkedEntities();
        renderer.render(&world);
        // Get a reference to the keyboard object
        auto &keyboard = getApp()->getKeyboard();

        if (keyboard.justPressed(GLFW_KEY_SPACE))
        {
            // If the space key is pressed in this frame, go to the play state
            getApp()->changeState("play");
        }
        if (keyboard.justPressed(GLFW_KEY_ESCAPE))
        {
            // If the escape key is pressed in this frame, quit the application
            getApp()->close();
        }
    }

    int player_index = 0;

    void onImmediateGui()
    {
        // some flags to make the window transparent and not movable
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration |
                                        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                                        ImGuiWindowFlags_NoSavedSettings |
                                        ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav |
                                        ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar;
        ImGui::Begin("Menu", nullptr, window_flags);

        ImGui::SetWindowFontScale(2);
        ImGui::Text("Welcome To The Game");
        ImGui::Text("Press space to Play");
        ImGui::Text("Press escape to Quit");
        ImGui::Text("Left and Right arrows to choose the player");

        ImGui::End();
        // write the player index in the window
        ImGui::Begin("Players", nullptr, window_flags);
        ImGui::SetCursorPos(ImVec2(110, 222));
        ImGui::Text("Player %d", previewController.getCurrentPlayer());

        // vertical slider to choose the player
        glm::vec2 window_size = getApp()->getFrameBufferSize();
        player_index = previewController.getCurrentPlayer();
        // Save the current style
        ImGuiStyle &style = ImGui::GetStyle();
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8)); // Increase padding
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);        // Add rounding to the slider

        // Set custom colors
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.4f, 0.6f, 1.0f));    // Background color
        ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.8f, 0.2f, 0.2f, 1.0f)); // Slider grab color

        // Display the slider
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y + 18));
        ImGui::VSliderInt("##v", ImVec2(18, 160), &player_index, 0, previewController.getPlayerCount() - 1);

        // Restore the previous style
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(2);

        if (player_index != previewController.getCurrentPlayer())
        {
            previewController.changePlayer(player_index);
        }
        ImGui::End();

        //        imgui tabs to show thee chosen player index from the players
    }

    void onDestroy() override
    {
        // Delete all the allocated resources
        world.clear();
        renderer.destroy();
        previewController.exit();
        our::clearAllAssets();
    }


};