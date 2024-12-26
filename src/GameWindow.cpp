#include "GameWindow.h"
#include <stdexcept>

namespace Game {

	GameWindow::GameWindow(int w, int h, std::string name) : width{ w }, height{ h }, windowName{ name } {
		initWindow();
	}

	GameWindow::~GameWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void GameWindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}

	void GameWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface)) {
			throw std::runtime_error("failed to create window surface");
		}
	}

	void GameWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto gameWindow = reinterpret_cast<GameWindow*>(glfwGetWindowUserPointer(window));
		gameWindow->framebufferResized = true;
		gameWindow->width = width;
		gameWindow->height = height;
	}

} // namespace Game