#include "ai_ui_manager.h"
#include "twist.h"
#include "face.h"
#include "cube_solver.h"
#include "scramble_generator.h"
#include <cstdlib>
#include <memory>
#include <boost/optional.hpp>
#include <iostream>

using namespace ui;

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
	cube -> set_frames_per_rotation(9);
	for (const auto& twist : cube::scramble_generator::generate_scramble(100, sym_cube.get_size())) {
		sym_cube.rotate(twist);
		cube -> rotate(twist);
	}
}

void AIUIManager::solve() {
	ai::CubeSolver solver;
	solver.add_twist_listener(this);
	solver.solve(sym_cube);
}
