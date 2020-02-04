#include "ai_ui_manager.h"
#include "twist.h"
#include "face.h"
#include "cube_solver.h"
#include <cstdlib>
#include <memory>
#include <boost/optional.hpp>
#include <iostream>

using namespace ui;

cube::Twist AIUIManager::random_twist() {
	int degrees = std::rand() % 2 ? 90 : -90;
	cube::Face face = cube::ALL_FACES[std::rand() % 6];
	int layer = std::rand() % size/2;
	bool wide_turn = std::rand() % 2;

	return cube::Twist(degrees, face, layer, wide_turn);
}

bool AIUIManager::keyPressed(const OgreBites::KeyboardEvent& event) {
	int ENTER = 13;
	if (event.keysym.sym == ENTER && !start_solution) {
		cube -> set_frames_per_rotation(3);
		for (const auto& twist : solution) {
			cube -> rotate(twist);
		}
		start_solution = true;
	}

	return true;
}

void AIUIManager::setup() {
	UIManager::setup();
	std::srand(std::time(nullptr));
	cube -> set_frames_per_rotation(9);
	for (int i = 0; i < 100; i++) {
		cube::Twist twist = random_twist();
		sym_cube.rotate(twist);
		cube -> rotate(twist);
	}
}

void AIUIManager::solve() {
	ai::CubeSolver solver;
	solver.add_twist_listener(this);
	solver.solve(sym_cube);
}
