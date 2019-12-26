#include <cstdlib>
#include <memory>
#include <boost/optional.hpp>

#include "ai_ui_manager.h"
#include "twist.h"
#include "face.h"
#include "center_solver.h"
#include "edge_solver.h"

using namespace ui;

cube::Twist AIUIManager::random_twist() {
	int degrees = std::rand() % 2 ? 90 : -90;
	cube::Face face = cube::ALL_FACES[std::rand() % 6];
	int layer = std::rand() % size/2;
	bool wide_turn = std::rand() % 2;

	return cube::Twist(degrees, face, layer, wide_turn);
}

void AIUIManager::setup() {
	UIManager::setup();
	std::srand(std::time(nullptr));

	for (int i = 0; i < 30; i++) {
		cube::Twist twist = random_twist();
		sym_cube_centers.rotate(twist);
		sym_cube.rotate(twist);
		cube -> rotate(twist);
	}

}

void AIUIManager::solve() {
	ai::CenterSolver center_solver;
	center_solver.add_twist_listener(this);
	center_solver.solve(sym_cube_centers);
	
	ai::EdgeSolver edge_solver;
	edge_solver.add_twist_listener(this);
	edge_solver.solve(sym_cube);
}
