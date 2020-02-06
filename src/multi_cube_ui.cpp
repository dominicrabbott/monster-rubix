#include "multi_cube_ui.h"
#include "combined_cube.h"
#include "scramble_generator.h"
#include "cube_solver.h"
#include <iostream>

using namespace ui;

void MultiCubeUI::SolutionExecutor::start_solution() {
	if (!solution_started) {
		solution_started = true;
		cube -> set_frames_per_rotation(3);
		std::unique_lock<std::mutex>(solution_mex);
		for (const auto& twist : solution) {
			cube -> rotate(twist);
		}
	}
}

MultiCubeUI::MultiCubeUI(const std::vector<int>& cube_sizes) : UIManager(1) {
	for (const int size : cube_sizes) {
		sym_cubes.push_back(cube::CombinedCube(size));
	}
}

void MultiCubeUI::setup() {
	UIManager::setup();
	auto* scene_mgr = getRoot() -> getSceneManagers().begin() -> second;
	
	auto* cube_root = cube -> get_root_node();
	cube_root -> getParentSceneNode() -> removeChild(cube_root);

	camera_man -> getCamera() -> setPosition(0,0,5000);
	
	int cube_spacing = 800;
	for (int cube_index = 0; cube_index < sym_cubes.size(); cube_index++) {
		auto cube = std::make_unique<ui::CubeDisplay>(sym_cubes[cube_index].get_size(), scene_mgr);
		addInputListener(cube.get());
		int cube_position = 800*cube_index - sym_cubes.size()/2*cube_spacing;
		cube -> get_root_node() -> setPosition(cube_position, 0, 0);
		for (const auto& twst : cube::scramble_generator::generate_scramble(100, sym_cubes[cube_index].get_size(), std::time(nullptr)+cube_index)) {
			cube -> rotate(twst);
			sym_cubes[cube_index].rotate(twst);
		}
		cube_displays.push_back(std::move(cube));
	}
}

bool MultiCubeUI::keyPressed(const OgreBites::KeyboardEvent& event) {
	int ENTER = 13;
	if (event.keysym.sym == ENTER) {
		std::unique_lock<std::mutex>(executor_mex);
		for (const auto& exec_ptr : executors) {
			exec_ptr -> start_solution();
		}
	}

	return true;
}

void MultiCubeUI::solve_cube(const int cube_index) {
	auto exec_ptr = std::make_unique<SolutionExecutor>(cube_displays[cube_index].get());
	ai::CubeSolver solver;
	solver.add_twist_listener(exec_ptr.get());
	{
		std::unique_lock<std::mutex>(executor_mex);
		executors.push_back(std::move(exec_ptr));
	}
	solver.solve(sym_cubes[cube_index]);
}
