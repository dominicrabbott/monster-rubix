#include <cstdlib>
#include <memory>
#include <iostream>
#include <boost/optional.hpp>

#include "ai_ui_manager.h"
#include "twist.h"
#include "face.h"
#include "three_cube_solver.h"

using namespace ui;

cube::Twist AIUIManager::random_twist() {
	int degrees = std::rand() % 2 ? 90 : -90;
	cube::Face face = cube::ALL_FACES[std::rand() % 6];
	int layer = 0;
	bool wide_turn = std::rand() % 2;

	return cube::Twist(degrees, face, layer, wide_turn);
}

void AIUIManager::setup() {
	UIManager::setup();
	std::srand(std::time(nullptr));

	for (int i = 0; i < 30; i++) {
		cube::Twist twist = random_twist();
		sym_cube.rotate(twist);
		cube -> rotate(twist);
	}

}

void AIUIManager::solve() {
	for (cube::Twist& twist : ai::ThreeCubeSolver().solve(sym_cube)) {
		cube -> rotate(twist);
	}
}
