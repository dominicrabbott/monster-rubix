#include "center_solver.h"
#include "cube_centers.h"
#include "face.h"
#include "search.h"
#include <stdexcept>
#include <queue>
#include <limits>
#include <iostream>

using namespace ai;

int CenterSolver::CenterHeuristic::operator()(const cube::CubeCenters& centers) {
	int heuristic_value = 0;
	int pieces_in_center = std::pow(centers.get_size()-2,2);
	for (const auto face : cube::ALL_FACES) {
		int face_start_index = static_cast<int>(face)*pieces_in_center;
		int solved_center_value = centers.get_solved_center_value(face);
		for (int piece = 0; piece < pieces_in_center; piece++) {
			int piece_value = centers.get_center_pos(face_start_index + piece);
			if (piece_value == centers.get_solved_center_value(cube::OPPOSING_FACES.at(face))) {
				heuristic_value += 2;
			}
			else if (piece_value != solved_center_value) {
				heuristic_value++;
			}
		}	
	}
	
	return heuristic_value;
}

std::vector<TwistSequence> CenterSolver::generate_commutators(const cube::CubeCenters& centers) {
	using namespace cube;
	
	std::vector<TwistSequence> commutators;
	int center_width = centers.get_size()-2;
	int bound_x = center_width/2;
	int bound_y = std::ceil(center_width/2.0f);
	for (int x = 0; x < bound_x; x++) {
		for (int y = 0; y < bound_y; y++) {
			TwistSequence comm({
					Twist(-90, Face::LEFT, x+1, false),
					Twist(-90, Face::TOP),
					Twist(-90, Face::LEFT, center_width-y, false),
					Twist(90, Face::TOP),
					Twist(90, Face::LEFT, x+1, false),
					Twist(-90, Face::TOP),
					Twist(90, Face::LEFT, center_width-y, false),
			});
			commutators.push_back(comm);
		}	
	}

	return commutators;
}

std::vector<TwistSequence> CenterSolver::generate_strategy_1(const cube::CubeCenters& centers) {
	std::vector<TwistSequence> strategy;

	std::array<cube::Face, 3> slice_faces = {cube::Face::LEFT, cube::Face::BOTTOM, cube::Face::BACK};
	std::array<int, 2> rotations = {90, -90};
	for (auto& face : slice_faces) {
		for (int slice = 0; slice < centers.get_size(); slice++) {
			for (const int degrees : rotations) {
				strategy.push_back(TwistSequence(cube::Twist(degrees, face, slice, false)));
			}
		}	
	}

	return strategy;
}

std::vector<TwistSequence> CenterSolver::generate_strategy_2(const cube::CubeCenters& centers) {
	using namespace cube;
	std::vector<TwistSequence> strategy;
	
	std::array<int, 2> rotations = {90, -90};
	std::array<cube::Face, 3> axes = {Face::LEFT, Face::BOTTOM, Face::BACK};
	for (const auto& face : ALL_FACES) {
		for (const int degrees : rotations) {
			strategy.push_back(TwistSequence(Twist(degrees, face)));	
		}
	}
	for (const auto& axis : axes) {
		for (const int degrees : rotations) {
			strategy.push_back(TwistSequence(Twist(degrees, axis, centers.get_size()-1)));
		}	
	}

	for (const auto& twist_seq : generate_commutators(centers)) {
		strategy.push_back(twist_seq);	
	}
	
	return strategy;
}

int CenterSolver::count_solved_pieces(const cube::CubeCenters& centers) {
	int placed_pieces = 0;
	for (const auto face : cube::ALL_FACES) {
		for (int i = 0; i < pieces_per_center(centers); i++) {
			int piece_index = static_cast<int>(face)*pieces_per_center(centers) + i;
			if (centers.get_center_pos(piece_index) == centers.get_solved_center_value(face)) {
				placed_pieces++;
			}
		}	
	}

	return placed_pieces;
}

void CenterSolver::solve(const cube::CubeCenters& root_state) {
	cube::CubeCenters curr_state(root_state);
	Search<cube::CubeCenters, CenterHeuristic> search;

	int states_searched = 0;
	int strategy_change_threshold = curr_state.get_size()*2500;
	auto strategy_1_finished = [strategy_change_threshold, states_searched=0] (const cube::CubeCenters& centers) mutable {
		states_searched++;
		return states_searched == strategy_change_threshold;
	};
	auto strategy_1_twists = search.best_first_search(curr_state, generate_strategy_1(curr_state), strategy_1_finished);
	notify_listeners(strategy_1_twists);
	for (const auto& twist : strategy_1_twists) {
		curr_state.rotate(twist);	
	}
	std::cout << "Strategy 1 finished. Starting strategy 2\n";
	auto strategy_2_finished = [this](const cube::CubeCenters& centers) {
		return this->count_solved_pieces(centers) == this->pieces_per_center(centers)*6;
	};
	notify_listeners(search.best_first_search(curr_state, generate_strategy_2(curr_state), strategy_2_finished));
	std::cout << "Done solving centers"  << std::endl;
}
