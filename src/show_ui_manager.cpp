#include "show_ui_manager.h"
#include "twist.h"
#include <iostream>

using namespace ui;

cube::Twist ShowUIManager::random_twist() {
	int degrees = std::rand() % 2 ? 90 : -90;
	cube::Face face = cube::ALL_FACES[std::rand() % 6];

	return cube::Twist(degrees, face);

}

bool ShowUIManager::keyPressed(const OgreBites::KeyboardEvent &event) {
	int ENTER = 13;
	if (event.keysym.sym == ENTER) {
		for (const auto& twist : solution) {
			cube -> rotate(twist);	
		}
	}

	return true;
}

void ShowUIManager::setup() {
	UIManager::setup();
	for (int i = 0; i < 30; i++) {
		auto twist = random_twist();
		sym_cube.rotate(twist);	
		cube -> rotate(twist);
	}
}

void ShowUIManager::solve() {
	solution = ai::ThreeCubeSolver().solve(sym_cube);
}