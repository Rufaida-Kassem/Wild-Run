#pragma once

#include <application.hpp>

#include <ecs/world.hpp>
#include <systems/forward-renderer.hpp>
#include <systems/free-camera-controller.hpp>
#include <systems/movement.hpp>
#include <systems/collision.hpp>
#include <asset-loader.hpp>

// This state shows how to use the ECS framework and deserialization.
class Playstate : public our::State
{

	our::World world;
	our::ForwardRenderer renderer;
	our::FreeCameraControllerSystem cameraController;
	our::MovementSystem movementSystem;
	our::CollisionSystem collisionSystem;

	void onInitialize() override
	{
		// First of all, we get the scene configuration from the app config
		auto& config = getApp()->getConfig()["scene"];
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
		collisionSystem.OnInitialize();
	}

	void onDraw(double deltaTime) override
	{
		// Here, we just run a bunch of systems to control the world logic
		movementSystem.update(&world, (float)deltaTime);
		cameraController.update(&world, (float)deltaTime);
		collisionSystem.update(&world, (float)deltaTime);
		world.deleteMarkedEntities();
		// And finally we use the renderer system to draw the scene
		renderer.render(&world);

		// Get a reference to the keyboard object
		auto& keyboard = getApp()->getKeyboard();

		if (keyboard.justPressed(GLFW_KEY_ESCAPE))
		{
			// If the escape  key is pressed in this frame, go to the play state
			getApp()->changeState("menu");
		}
		if (collisionSystem.get_is_lost()) {
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
		const std::string current_coins = "Coins: " + std::to_string(collisionSystem.get_coins_collected());
		// resize the window
		ImGui::SetWindowSize(ImVec2(100, 100));
		ImGui::Text(current_coins.c_str());
		ImGui::End();
	}
	void onDestroy() override
	{
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