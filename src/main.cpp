#include <thread>
#include <iostream>
#include <cstdlib>
#include <cctype>
#include "ai_ui_manager.h"

int main(int argc, char* argv[]) {
	if (argc != 2 || !std::all_of(argv[1], argv[1] + std::strlen(argv[1]), ::isdigit)) {
		std::cout << "Usage : MonsterRubix <cube size>\n";
		return 1;	
	}
	int cube_size = std::atoi(argv[1]);
	if (cube_size < 3) {
		std::cout << "Please provide a size greater than 3\n";
		return 1;
	}

	ui::AIUIManager man(cube_size);
	man.initApp();
	std::thread solve_thread(&ui::AIUIManager::solve, &man);
	man.getRoot() -> startRendering();
	man.closeApp();
	solve_thread.join();
}
