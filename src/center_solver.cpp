#include "center_solver.h"
#include "cube_centers.h"
#include "face.h"
#include "search.h"
#include "twist_utils.h"
#include <stdexcept>
#include <queue>
#include <limits>
#include <iostream>

using namespace ai;

int CenterSolver::CenterHeuristic::operator()(const cube::CubeCenters& centers) {
	int heuristic_value = 0;
	for (const auto face : cube::ALL_FACES) {
		int face_start_index = static_cast<int>(face)*centers.get_pieces_in_center();
		int solved_center_value = centers.get_solved_center_value(face);
		for (int piece = 0; piece < centers.get_pieces_in_center(); piece++) {
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
	int bound_x = centers.get_edge_width()/2;
	int bound_y = std::ceil(centers.get_edge_width()/2.0f);
	for (int x = 0; x < bound_x; x++) {
		for (int y = 0; y < bound_y; y++) {
			TwistSequence comm({
					Twist(-90, Face::LEFT, x+1, false),
					Twist(-90, Face::TOP),
					Twist(-90, Face::LEFT, centers.get_edge_width()-y, false),
					Twist(90, Face::TOP),
					Twist(90, Face::LEFT, x+1, false),
					Twist(-90, Face::TOP),
					Twist(90, Face::LEFT, centers.get_edge_width()-y, false),
			});
			commutators.push_back(comm);
		}	
	}

	return commutators;
}

std::vector<TwistSequence> CenterSolver::generate_strategy_1(const cube::CubeCenters& centers) {
	std::vector<TwistSequence> strategy;

	for (auto& face : TwistUtils::AXIS_FACES) {
		for (int slice = 0; slice < centers.get_size(); slice++) {
			for (const int deg : TwistUtils::DEGREES) {
				strategy.push_back(TwistSequence({cube::Twist(deg, face, slice, false)}));
			}
		}	
	}

	return strategy;
}

std::vector<TwistSequence> CenterSolver::generate_strategy_2(const cube::CubeCenters& centers) {
	using namespace cube;
	std::vector<TwistSequence> strategy;
	
	for (const auto& face : ALL_FACES) {
		for (const int degrees : TwistUtils::DEGREES) {
			strategy.push_back(TwistSequence({Twist(degrees, face)}));	
		}
	}
	for (const auto& axis : TwistUtils::AXIS_FACES) {
		for (const int deg : TwistUtils::DEGREES) {
			strategy.push_back(TwistSequence({Twist(deg, axis, centers.get_size()-1)}));
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
		for (int i = 0; i < centers.get_pieces_in_center(); i++) {
			int piece_index = static_cast<int>(face)*centers.get_pieces_in_center() + i;
			if (centers.get_center_pos(piece_index) == centers.get_solved_center_value(face)) {
				placed_pieces++;
			}
		}	
	}

	return placed_pieces;
}

void CenterSolver::solve(const cube::CubeCenters& root_state) {
	cube::CubeCenters curr_state(root_state);
	int states_searched = 0;
	int strategy_change_threshold = curr_state.get_size()*2500;
	int total_center_pieces = curr_state.get_pieces_in_center()*6;
	auto strategy_1_finished = [strategy_change_threshold, states_searched=0, total_center_pieces, this] (const cube::CubeCenters& centers) mutable {
		states_searched++;
		return states_searched == strategy_change_threshold || this->count_solved_pieces(centers) == total_center_pieces;
	};
	std::cout << "Beginning solving the centers using strategy 1\n";
	auto strategy_1_twists = search::best_first_search<cube::CubeCenters, CenterHeuristic>(curr_state, generate_strategy_1(curr_state), strategy_1_finished);
	notify_listeners(strategy_1_twists);
	for (const auto& twist : strategy_1_twists) {
		curr_state.rotate(twist);	
	}
	std::cout << "Strategy 1 finished. Beginning commutator based search\n";
	auto strategy_2_finished = [this, total_center_pieces](const cube::CubeCenters& centers) {
		return this->count_solved_pieces(centers) == total_center_pieces;
	};
	notify_listeners(search::best_first_search<cube::CubeCenters, CenterHeuristic>(curr_state, generate_strategy_2(curr_state), strategy_2_finished));

	std::cout << "Finished solving centers\n";
}
