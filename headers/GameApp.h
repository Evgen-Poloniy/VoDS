#pragma once

#include "GameDevice.h"
#include "GamePipeLine.h"
#include "GameSwapChain.h"
#include "GameWindow.h"
#include "GameModel.h"

// std
#include <memory>
#include <vector>

#include <chrono> 
#include <thread>

namespace Game {
	class GameApp {
	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;
		GameApp();
		~GameApp();

		GameApp(const GameApp&) = delete;
		GameApp& operator=(const GameApp&) = delete;

		void Run();

	private:
		void loadModels();
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void freeCommandBuffers();
		void drawFrame();
		void recreateSwapChain();
		void recordCommandBuffer(int imageIndex);

		void KeyboardInput();
		bool enter_pressed = false;

		inline std::chrono::time_point<std::chrono::high_resolution_clock> StartTime();
		void ShowFPS(const std::chrono::time_point<std::chrono::high_resolution_clock>& start_time);
		bool showFPS_isOn = false;

		GameWindow gameWindow{ WIDTH, HEIGHT, "VoDS" };
		GameDevice gameDevice{ gameWindow };
		std::unique_ptr<GameSwapChain> gameSwapChain;
		std::unique_ptr<GamePipeline> gamePipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		std::unique_ptr<GameModel> gameModel;

	};
} // namespace Game