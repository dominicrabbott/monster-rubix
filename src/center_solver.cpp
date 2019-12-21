#include "center_solver.h"
#include "cube_centers.h"
#include "face.h"
#include <cmath>
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

std::vector<cube::Twist> CenterSolver::trace_twists(const HeuristicCubeState<cube::CubeCenters, CenterSolver::CenterHeuristic>* state) {
	std::vector<cube::Twist> twists;
	const State* curr_state = state;
	while (curr_state->parent != nullptr) {
		auto& curr_twists = curr_state->twist_seq->twists;
		for (auto it = curr_twists.rbegin(); it != curr_twists.rend(); it++) {
			twists.push_back(*it);	
		}
		curr_state = curr_state->parent.get();
	}
	std::reverse(twists.begin(), twists.end());
	std::cout << "Solved in " << twists.size() << " twists"  << std::endl;
	return twists;
}

void CenterSolver::rotate(const std::vector<cube::Twist>& twists) {
	for (const auto& twst : twists) {
		for (auto listener_ptr : twist_listeners) {
			listener_ptr->twist(twst);	
		}
	}
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

void CenterSolver::solve(const cube::CubeCenters& centers) {
	int solved_heuristic_value = 0;
	auto state_compare = [](const std::shared_ptr<State> state1, const std::shared_ptr<State> state2) {
		return state1->score > state2->score;
	};
	std::priority_queue<std::shared_ptr<State>, std::vector<std::shared_ptr<State>>, decltype(state_compare)> open(state_compare);
	open.push(std::make_shared<State>(centers));
	std::unordered_set<cube::CubeCenters> seen = {centers};
	auto twist_sequences = generate_strategy_1(centers);
	
	int states_searched = 0;
	int strategy_change_threshold = centers.get_size()*3000;
	int best_heuristic_score = std::numeric_limits<int>::max();
	std::shared_ptr<State> best_state;
	while (!open.empty()) {
		auto curr_state = open.top();
		open.pop();
		for (const auto& twist_seq : twist_sequences) {
			cube::CubeCenters child_cube(curr_state->cube);
			for (const auto& twist : twist_seq.twists) {
				child_cube.rotate(twist);
			}
			if (!seen.count(child_cube)) {
				seen.insert(child_cube);
				auto child_state = std::make_shared<State>(curr_state, std::move(child_cube), twist_seq);
				int heuristic_value = child_state->score;
				if (heuristic_value < best_heuristic_score) {
					best_heuristic_score = heuristic_value;
					best_state = child_state;
					std::cout << "Scored " << best_heuristic_score  << std::endl;
				}
				if (heuristic_value == solved_heuristic_value) {
					for (const auto& twst : trace_twists(child_state.get())) {
						for (auto listener_ptr : twist_listeners) {
							listener_ptr->twist(twst);	
						}
					}
					return;
				}
				open.push(child_state);
				states_searched++;
				if (states_searched == strategy_change_threshold) {
					twist_sequences = generate_strategy_2(centers);
					while (!open.empty()) {
						open.pop();	
					}
					open.push(best_state);
					std::cout << "Changing Strategy. Best achived: "  << best_heuristic_score << std::endl;
					break;
				}
			}
		}
	}

	throw std::invalid_argument("The given cube couldn't be solved");
}
