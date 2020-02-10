#include <thread>
#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>
#include "multi_cube_ui.h"
#include "keyboard_ui_manager.h"

void start_ui(const std::vector<int>& cube_sizes) {
	ui::MultiCubeUI ui(cube_sizes);
	ui.initApp();
	std::vector<std::thread> solve_threads;
	for (int i = 0; i < cube_sizes.size(); i++) {
		solve_threads.emplace_back(&ui::MultiCubeUI::solve_cube, &ui, i);
	}
	ui.getRoot() -> startRendering();
	ui.closeApp();
	for (auto& th : solve_threads) {
		th.join();
	}

}

bool is_number(char* str) {
	return std::all_of(str, str+std::strlen(str), ::isdigit);
}

int main(int argc, char* argv[]) {
	std::vector<int> cube_sizes;
	std::string usage_message = "Usage : MonsterRubix [keyboard | <cube 1 size>, <cube 2 size>, ...] \n";

	if (argc == 2 && !strcmp("keyboard", argv[1])) {
		ui::KeyboardUIManager ui;
		ui.initApp();
		ui.getRoot() -> startRendering();
		ui.closeApp();
	}
	else {
		for (int arg_index = 1; arg_index < argc; arg_index++) {
			if (!is_number(argv[arg_index])) {
				std::cout << "All arguments must be numbers\n" << usage_message;
				return 1;
			}
			int cube_size = std::atoi(argv[arg_index]);
			if (cube_size < 3 || cube_size > 9) {
				std::cout << "Please provide sizes greater than 3 and less than 10\n" << usage_message;
				return 1;
			}
			cube_sizes.push_back(cube_size);
		}
		if (cube_sizes.size() == 0) {
			std::cout << "Please provide at least one cube size\n" << usage_message;
			return 1;
		}
		start_ui(cube_sizes);
	}
}
