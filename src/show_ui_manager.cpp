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
	cube -> set_frames_per_rotation(3);
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
	cube -> set_frames_per_rotation(10);
	for (int i = 0; i < 30; i++) {
		auto twist = random_twist();
		sym_cube.rotate(twist);	
		cube -> rotate(twist);
	}
}

void ShowUIManager::solve() {
	ai::ThreeCubeSolver solver;
	solver.add_twist_listener(this);
	solver.solve(sym_cube);
}
