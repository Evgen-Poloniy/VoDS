#include "GameApp.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

//std
#include <array>
#include <cassert>
#include <stdexcept>
#include <iomanip>

namespace Game {

    struct SimplePushConstantData {
        glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };

    struct PlayerMovement {
        float X{};
        float Y{};
    };
    PlayerMovement playerMovement{};
    float speedOfMovement{ 0.001f };

    GameApp::GameApp() {
        loadModels();
        glfwMakeContextCurrent(gameWindow.GetWindow());
        createPipelineLayout();
        //createPipeline();
        recreateSwapChain();
        createCommandBuffers();
    }

    GameApp::~GameApp() { vkDestroyPipelineLayout(gameDevice.device(), pipelineLayout, nullptr); }

    void GameApp::KeyboardInput() {
        if (glfwGetKey(gameWindow.GetWindow(), GLFW_KEY_ENTER) == GLFW_PRESS) {
            if (!enter_pressed) {
                std::cout << "Inputed key Enter" << std::endl;
                enter_pressed = true;
            }
        }
        else {
            enter_pressed = false;
        }
        if (glfwGetKey(gameWindow.GetWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(gameWindow.GetWindow(), true);
        }
        if (glfwGetKey(gameWindow.GetWindow(), GLFW_KEY_F10) == GLFW_PRESS) {
            glfwSetWindowSize(gameWindow.GetWindow(), 1920, 1080);
        }
        if (glfwGetKey(gameWindow.GetWindow(), GLFW_KEY_F3) == GLFW_PRESS) {

        }
        if (glfwGetKey(gameWindow.GetWindow(), GLFW_KEY_W) == GLFW_PRESS) {
            playerMovement.Y -= speedOfMovement;
        }
        if (glfwGetKey(gameWindow.GetWindow(), GLFW_KEY_S) == GLFW_PRESS) {
            playerMovement.Y += speedOfMovement;
        }
        if (glfwGetKey(gameWindow.GetWindow(), GLFW_KEY_A) == GLFW_PRESS) {
            playerMovement.X -= speedOfMovement;
        }
        if (glfwGetKey(gameWindow.GetWindow(), GLFW_KEY_D) == GLFW_PRESS) {
            playerMovement.X += speedOfMovement;
        }
    }

    inline std::chrono::time_point<std::chrono::high_resolution_clock> GameApp::StartTime() {
        return std::chrono::high_resolution_clock::now();
    }

    void GameApp::ShowFPS(const std::chrono::time_point<std::chrono::high_resolution_clock>& start_time) {
        if (showFPS_isOn)
        {
            auto end_time = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> elapsed = end_time - start_time;
            if (elapsed.count() < 16.67) {
                std::this_thread::sleep_for(std::chrono::milliseconds(16) - elapsed);
                std::cout << "FPS: " << 60 << std::endl;
            }
            else {
                std::cout << "FPS: " << 1000 / elapsed.count() << std::setprecision(3) << std::endl;
            }
        }
    }

    void GameApp::Run() {
        while (!gameWindow.shouldClose()) {
            auto start_time = StartTime();
            KeyboardInput();
            glfwPollEvents();
            drawFrame();
            ShowFPS(start_time);
        }

        vkDeviceWaitIdle(gameDevice.device());

    }

    void GameApp::loadModels() {
        std::vector<GameModel::Vertex> vertices{
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
        };
        gameModel = std::make_unique<GameModel>(gameDevice, vertices);
    }

    void GameApp::createPipelineLayout() {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
        if (vkCreatePipelineLayout(gameDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void GameApp::recreateSwapChain() {
        auto extent = gameWindow.getExtent();
        while (extent.width == 0 || extent.height == 0) {
            extent = gameWindow.getExtent();
            glfwWaitEvents();
        }
        vkDeviceWaitIdle(gameDevice.device());

        if (gameSwapChain == nullptr) {
            gameSwapChain = std::make_unique<GameSwapChain>(gameDevice, extent);
        }
        else {
            gameSwapChain = std::make_unique<GameSwapChain>(gameDevice, extent, std::move(gameSwapChain));
            if (gameSwapChain->imageCount() != commandBuffers.size()) {
                freeCommandBuffers();
                createCommandBuffers();
            }
        }

        createPipeline();
    }

    void GameApp::createPipeline() {
        assert(gameSwapChain != nullptr && "Cannot create pipeline before swap chain");
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        GamePipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = gameSwapChain->getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        gamePipeline = std::make_unique<GamePipeline>(
            gameDevice,
            "/home/evgen-poloniy/snap/code/projects/VoDS/Shaders/Shader.vert.spv",
            "/home/evgen-poloniy/snap/code/projects/VoDS/Shaders/Shader.frag.spv",
            pipelineConfig);
    }

    void GameApp::createCommandBuffers() {
        commandBuffers.resize(gameSwapChain->imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = gameDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
        if (vkAllocateCommandBuffers(gameDevice.device(), &allocInfo, commandBuffers.data()) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }

    void GameApp::freeCommandBuffers() {
        vkFreeCommandBuffers(
            gameDevice.device(),
            gameDevice.getCommandPool(),
            static_cast<uint32_t>(commandBuffers.size()),
            commandBuffers.data());
        commandBuffers.clear();
    }

    void GameApp::recordCommandBuffer(int imageIndex) {
        //static int frame = 30;
        //frame = (frame + 1) % 100;

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = gameSwapChain->getRenderPass();
        renderPassInfo.framebuffer = gameSwapChain->getFrameBuffer(imageIndex);

        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = gameSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        //clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
        clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(gameSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(gameSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{ {0, 0}, gameSwapChain->getSwapChainExtent() };
        vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
        vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

        gamePipeline->bind(commandBuffers[imageIndex]);
        gameModel->bind(commandBuffers[imageIndex]);
        //gameModel->draw(commandBuffers[imageIndex]);

        /* ������������ � �����
        for (int j = 0; j < 4; j++) {
            SimplePushConstantData push{};
            push.offset = { -0.5f + frame * 0.02f, -0.4f + j * 0.25f };
            push.color = { 0.0f, 0.0f, 0.2f + 0.2f * j };

            vkCmdPushConstants(
                commandBuffers[imageIndex],
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push);
            gameModel->draw(commandBuffers[imageIndex]);
        }
        */

        SimplePushConstantData push{};
        push.offset = { playerMovement.X, playerMovement.Y };
        push.color = { 0.0f, 0.0f, 0.2f + 0.2f };

        //playerMovement.Y = 0.0f;
        //playerMovement.X = 0.0f;

        vkCmdPushConstants(
            commandBuffers[imageIndex],
            pipelineLayout,
            VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
            0,
            sizeof(SimplePushConstantData),
            &push);
        gameModel->draw(commandBuffers[imageIndex]);

        vkCmdEndRenderPass(commandBuffers[imageIndex]);
        if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    void GameApp::drawFrame() {
        uint32_t imageIndex;
        auto result = gameSwapChain->acquireNextImage(&imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }
        recordCommandBuffer(imageIndex);
        result = gameSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
            gameWindow.wasWindowResized()) {
            gameWindow.resetWindowResizedFlag();
            recreateSwapChain();
            return;
        }
        else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }
    }

} // namespace Game