#include "GameApp.h"
// std
#include <cstdlib>
#include <stdexcept>

int main() {
	Game::GameApp game{};
	try {
		game.Run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}