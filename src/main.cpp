#include <thread>

#include "show_ui_manager.h"

int main() {
	ui::ShowUIManager man;
	man.initApp();
	std::thread load_thread(&ui::ShowUIManager::solve, &man);
	man.getRoot() -> startRendering();
	man.closeApp();
	load_thread.join();
}
