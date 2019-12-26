#include "edge_solver.h"
#include "hash.h"
#include "search.h"
#include <unordered_set>
#include <algorithm>
#include <queue>
#include <iostream>
#include <optional>

using namespace ai;

int EdgeSolver::EdgeHeuristic::operator()(const cube::Cube& cube) {
	int edge_width = cube.get_size()-2;
	int edge_count = 12;

	auto edge_assignment_compare = [](const EdgeAssignment& lhs, const EdgeAssignment& rhs) {
		return lhs.placed_pieces < rhs.placed_pieces;	
	};
	auto map_value_compare = [](const std::pair<int, int>& lhs, const std::pair<int, int>& rhs) {
		return lhs.second < rhs.second;	
	};
	std::priority_queue<EdgeAssignment, std::vector<EdgeAssignment>, decltype(edge_assignment_compare)> possible_assignments(edge_assignment_compare);
	for (int piece = 0; piece < edge_count; piece++) {
		for (int edge = 0; edge < edge_count; edge++) {
			std::unordered_map<int, int> piece_orientation_counts;
			for (int i = 0; i < edge_width; i++) {
				int position = edge*edge_width+i;
				if (cube.get_edge_pos(position) == piece) {
					piece_orientation_counts[cube.get_edge_orientation(position)]++;
				}
			}
			int placed_pieces = 0;
			if (!piece_orientation_counts.empty()) {
				placed_pieces = std::max_element(piece_orientation_counts.begin(), piece_orientation_counts.end(), map_value_compare) -> second;
			
			}
			possible_assignments.push(EdgeAssignment(edge, piece, placed_pieces));
		}
	}
	std::unordered_set<int> unassigned_edges;
	std::unordered_set<int> unassigned_pieces;
	for (int i = 0; i < edge_count; i++) {
		unassigned_edges.insert(i);
		unassigned_pieces.insert(i);
	}
	int total_pieces_placed = 0;
	while (!unassigned_edges.empty()) {
		auto assignment = possible_assignments.top();
		if (unassigned_edges.count(assignment.edge) && unassigned_pieces.count(assignment.piece)) {
			unassigned_edges.erase(assignment.edge);
			unassigned_pieces.erase(assignment.piece);
			total_pieces_placed += assignment.placed_pieces;
		}
		possible_assignments.pop();
	}
	return (edge_width*edge_count) - total_pieces_placed;
}

int EdgeSolver::LastTwoEdgesHeuristic::operator()(const cube::Cube& cube) {
	EdgeSolver solver;
	std::unordered_set<int> top_edges = {8, 10};
	int score = 0;
	for (int edge = 0; edge < solver.edge_count; edge++) {
		if (!top_edges.count(edge) && !solver.edge_is_solved(cube, edge)) {
			score++;
		}
	}

	return score;
}

std::vector<TwistSequence> EdgeSolver::generate_edge_commutators(const cube::Cube& cube) {
	using namespace cube;
	std::vector<TwistSequence> commutators;
	for (int edge = 1; edge < cube.get_size()-1; edge++) {
		boost::optional<TwistSequence> comm;
		if (cube.get_size()%2 != 0 && edge == cube.get_size()/2) {
			int alg_layer = cube.get_size()/2 - 1;
			comm = TwistSequence({
					Twist(90, Face::FRONT),
					Twist(90, Face::FRONT),
					Twist(90, Face::RIGHT, alg_layer),
					Twist(90, Face::BOTTOM),
					Twist(90, Face::BOTTOM),
					Twist(-90, Face::RIGHT, alg_layer),
					Twist(90, Face::FRONT),
					Twist(90, Face::FRONT),
					Twist(90, Face::TOP),
					Twist(90, Face::TOP),
					Twist(90, Face::FRONT),
					Twist(90, Face::FRONT),
					Twist(90, Face::LEFT, alg_layer),
					Twist(90, Face::BACK),
					Twist(90, Face::BACK),
					Twist(-90, Face::LEFT, alg_layer),
			});
		}
		else {
			comm = TwistSequence({
				Twist(-90, Face::LEFT, edge, false),		
				Twist(90, Face::TOP),		
				Twist(90, Face::TOP),
				Twist(-90, Face::LEFT, edge, false),		
				Twist(90, Face::TOP),		
				Twist(90, Face::TOP),
				Twist(90, Face::FRONT),		
				Twist(90, Face::FRONT),
				Twist(-90, Face::LEFT, edge, false),		
				Twist(90, Face::FRONT),		
				Twist(90, Face::FRONT),
				Twist(90, Face::RIGHT, edge, false),
				Twist(90, Face::TOP),		
				Twist(90, Face::TOP),
				Twist(-90, Face::RIGHT, edge, false),
				Twist(90, Face::TOP),		
				Twist(90, Face::TOP),
				Twist(90, Face::LEFT, edge, false),		
				Twist(90, Face::LEFT, edge, false),		

			});
		}
		commutators.push_back(*comm);
	}

	return commutators;
}

TwistSequence EdgeSolver::generate_edge_flipper(const cube::Cube& cube) {
	using namespace cube;
	int middle = cube.get_size()/2 - 1;
	return TwistSequence({
		Twist(90, Face::RIGHT, middle, false),
		Twist(90, Face::RIGHT, middle, false),
		Twist(90, Face::BACK),
		Twist(90, Face::BACK),
		Twist(90, Face::TOP),
		Twist(90, Face::TOP),
		Twist(90, Face::LEFT, middle, false),
		Twist(90, Face::TOP),
		Twist(90, Face::TOP),
		Twist(-90, Face::RIGHT, middle, false),
		Twist(90, Face::TOP),
		Twist(90, Face::TOP),
		Twist(90, Face::RIGHT, middle, false),
		Twist(90, Face::TOP),
		Twist(90, Face::TOP),
		Twist(90, Face::FRONT),
		Twist(90, Face::FRONT),
		Twist(90, Face::RIGHT, middle, false),
		Twist(90, Face::FRONT),
		Twist(90, Face::FRONT),
		Twist(-90, Face::LEFT, middle, false),
		Twist(90, Face::BACK),
		Twist(90, Face::BACK),
		Twist(90, Face::RIGHT, middle, false),
		Twist(90, Face::RIGHT, middle, false),
	});	

}

bool EdgeSolver::edge_is_solved(const cube::Cube& cube, const int edge) {
	int edge_width = cube.get_size()-2;
	int edge_start_index = edge*edge_width;
	int edge_pos = cube.get_edge_pos(edge_start_index);
	int edge_orientation = cube.get_edge_orientation(edge_start_index);
	for (int i = 1; i < edge_width; i++) {
		int edge_index = edge_start_index + i;
		if (cube.get_edge_pos(edge_index) != edge_pos || cube.get_edge_orientation(edge_index) != edge_orientation) {
			return false;
		}
	}

	return true;
}

std::vector<TwistSequence> EdgeSolver::generate_all_face_twists() {
	std::vector<TwistSequence> twist_sequences;
	for (const auto face : cube::ALL_FACES) {
		for (const int deg : degrees) {
			twist_sequences.push_back(TwistSequence({cube::Twist(deg, face)}));
		}	
	}

	return twist_sequences;
}

std::vector<cube::Twist> EdgeSolver::solve_first_ten_edges(const cube::Cube& cube) {
	using namespace cube;
	std::array<Face, 3> axis_faces = {Face::LEFT, Face::BOTTOM, Face::BACK};
	
	std::vector<TwistSequence> twist_sequences;
	auto edge_commutators = generate_edge_commutators(cube);
	twist_sequences.insert(twist_sequences.end(), edge_commutators.begin(), edge_commutators.end());
	auto face_rotations = generate_all_face_twists();
	twist_sequences.insert(twist_sequences.end(), face_rotations.begin(), face_rotations.end());

	for (const auto axis : axis_faces) {
		for (const int deg : degrees) {
			twist_sequences.push_back(TwistSequence({Twist(deg, axis, cube.get_size()-1)}));
		}	
	}
	
	auto is_finished = [this](const cube::Cube& cube) {
		int unsolved_edges = 0;
		for (int i = 0; i < this->edge_count; i++) {
			if (!this->edge_is_solved(cube, i)) {
				unsolved_edges++;
				if (unsolved_edges > 2) {
					return false;	
				}
			}
		}

		return true;
	};
	
	return search::best_first_search<cube::Cube, EdgeHeuristic>(cube, twist_sequences, is_finished);
}

std::vector<cube::Twist> EdgeSolver::solve_last_two_edges(const cube::Cube& cube) {
	using namespace cube;
	auto twist_sequences = generate_edge_commutators(cube);
	if (cube.get_size()%2 != 0) {
		twist_sequences.push_back(generate_edge_flipper(cube));
	}
	auto face_rotations = generate_all_face_twists();
	twist_sequences.insert(twist_sequences.end(), face_rotations.begin(), face_rotations.end());
	twist_sequences.push_back({
		Twist(90, Face::FRONT),
		Twist(-90, Face::TOP),
		Twist(90, Face::RIGHT),
		Twist(90, Face::TOP),
	});

	auto is_finished = [this](const cube::Cube& cube) {
		for (int edge = 0; edge < this->edge_count; edge++) {
			if (!this->edge_is_solved(cube, edge)) {
				return false;	
			}
		}

		return true;
	};
	
	return search::best_first_search<cube::Cube, LastTwoEdgesHeuristic>(cube, twist_sequences, is_finished);
}

void EdgeSolver::solve(const cube::Cube& cube) {
	cube::Cube current_state(cube);

	std::cout << "Solving the first 10 edges\n";
	auto partial_solution = solve_first_ten_edges(current_state);
	for (const auto& twist : partial_solution) {
		current_state.rotate(twist);
	}
	notify_listeners(partial_solution);
	std::cout << "First 10 edges solved. Solving the last 2 edges\n";
	notify_listeners(solve_last_two_edges(current_state));
	std::cout << "All edges solved!\n";
}
