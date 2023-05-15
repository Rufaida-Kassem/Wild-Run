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

// This struct is used to store the location and size of a button and the code it should execute when clicked
struct Button {
    // The position (of the top-left corner) of the button and its size in pixels
    glm::vec2 position, size;
    // The function that should be excuted when the button is clicked. It takes no arguments and returns nothing.
    std::function<void()> action;

    // This function returns true if the given vector v is inside the button. Otherwise, false is returned.
    // This is used to check if the mouse is hovering over the button.
    bool isInside(const glm::vec2 &v) const {
        return position.x <= v.x && position.y <= v.y &&
               v.x <= position.x + size.x &&
               v.y <= position.y + size.y;
    }

    // This function returns the local to world matrix to transform a rectangle of size 1x1
    // (and whose top-left corner is at the origin) to be the button.
    glm::mat4 getLocalToWorld() const {
        return glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, 0.0f)) *
               glm::scale(glm::mat4(1.0f), glm::vec3(size.x, size.y, 1.0f));
    }
};

// This state shows how to use some of the abstractions we created to make a menu.
class Menustate : public our::State {

    float time;
    std::array<Button, 2> buttons;
    //to render a background image
    our::ForwardRenderer renderer;
    our::World world;
    our::MovementSystem movementSystem;

    our::PreviewCameraControllerSystem previewController;

    void onInitialize() override {
        // First of all, we get the scene configuration from the app config
        auto &config = getApp()->getConfig()["menu-scene"];
        // If we have assets in the scene config, we deserialize them
        if (config.contains("assets")) {
            our::deserializeAllAssets(config["assets"]);
        }
        // If we have a world in the scene config, we use it to populate our world
        if (config.contains("world")) {
            world.deserialize(config["world"]);
        }
        // We initialize the camera controller system since it needs a pointer to the app
        // Then we initialize the renderer
        auto size = getApp()->getFrameBufferSize();
        renderer.initialize(size, config["renderer"]);
        previewController.enter(getApp(), &world);
        previewController.deserializePlayers(config["players-entities"]);
    }

    void onDraw(double deltaTime) override {
        previewController.update((float) deltaTime);
        movementSystem.update(&world, (float) deltaTime);


        world.deleteMarkedEntities();
        renderer.render(&world);
        // Get a reference to the keyboard object
        auto &keyboard = getApp()->getKeyboard();


        if (keyboard.justPressed(GLFW_KEY_SPACE)) {
            // If the space key is pressed in this frame, go to the play state
            getApp()->changeState("play");
        }
        if (keyboard.justPressed(GLFW_KEY_ESCAPE)) {
            // If the escape key is pressed in this frame, quit the application
            getApp()->close();
        }
    }

    void onDestroy() override {
        // Delete all the allocated resources
        world.clear();
        renderer.destroy();
        previewController.exit();
        our::clearAllAssets();
    }
};