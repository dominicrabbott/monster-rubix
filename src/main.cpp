#include <thread>

#include "ai_ui_manager.h"

int main() {
	ui::AIUIManager man(3);
	man.initApp();
	std::thread solve_thread(&ui::AIUIManager::solve, &man);
	man.getRoot() -> startRendering();
	man.closeApp();
	solve_thread.join();
}
