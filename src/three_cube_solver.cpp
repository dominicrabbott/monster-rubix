#include "three_cube_solver.h"
#include "cube.h"
#include "cube_centers.h"
#include "face.h"
#include "twist_utils.h"
#include "search.h"
#include "hash.h"
#include <queue>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

using namespace ai;
ThreeCubeSolver::ThreeCubeSolver() {
	using namespace cube;
	std::array<std::string, stage_count> filenames = {
		"g1_table.bin",
		"g2_table.bin",
		"g3_table.bin",
		"g4_table.bin",
	};

	std::array<std::unordered_set<cube::Face>, stage_count> restricted_faces = {{
		{},
		{Face::TOP, Face::BOTTOM},	
		{Face::TOP, Face::BOTTOM, Face::FRONT, Face::BACK},
		{Face::TOP, Face::BOTTOM, Face::FRONT, Face::BACK, Face::LEFT, Face::RIGHT},
	}};

	for (int stage = 0; stage < stage_count; stage++) {
		auto file_path = table_dir+filenames[stage];
		if (!load_lookup_table(tables[stage], file_path)) {
			tables[stage] = create_lookup_table(encoders[stage], generate_twist_sequences(restricted_faces[stage]));
			save_lookup_table(tables[stage], file_path);
			std::cout << "Table for stage " << stage << " generated\n";
		}
		else {
			std::cout << "Loading table for stage " << stage << " complete\n";
		}
	}
}

std::vector<cube::Twist> ThreeCubeSolver::orient_cube(const cube::CubeCenters& centers) {
	auto is_finished = [](const cube::CubeCenters& centers) {
		for (const auto face : TwistUtils::AXIS_FACES) {
			if (centers.get_solved_center_value(face) != static_cast<int>(face)) {
				return false;	
			}	
		}

		return true;
	};

	return search::breadth_first_search<cube::CubeCenters>(centers, TwistUtils::generate_cube_rotations(centers), is_finished);
}

bool ThreeCubeSolver::even_corner_parity(const cube::Cube& cube) {
	std::vector<int> corner_positions;
	for (int corner = 0; corner < cube.get_corner_count(); corner++) {
		corner_positions.push_back(cube.get_corner_pos(corner));	
	}

	return even_parity(corner_positions);
}

bool ThreeCubeSolver::even_edge_parity(const cube::Cube& cube) {
	std::vector<int> edge_positions;
	for (int edge = 0; edge < cube.get_edge_count(); edge++) {
		edge_positions.push_back(get_edge_pos(cube, edge));	
	}

	return even_parity(edge_positions);
}

bool ThreeCubeSolver::even_parity(const std::vector<int>& sequence) {
	auto sequence_copy = sequence;
	int cycles = 0;
	int start_index = 0;
	int marked_value = sequence.size();
	while (start_index < sequence_copy.size()) {
		if (sequence_copy[start_index] < marked_value) {
			int cycle_index = start_index;
			while (sequence_copy[cycle_index] != marked_value) {
				int& element = sequence_copy[cycle_index];
				cycle_index = element;
				element = marked_value;
			}
			cycles++;	
		}
		start_index++;
	}

	return (sequence.size()-cycles)%2 == 0;

}

TwistSequence ThreeCubeSolver::solve_parity(const cube::Cube& cube) {
	using namespace cube;
	int flipped_edges = 0;
	for (int edge = 0; edge < cube.get_edge_count(); edge++) {
		if (get_edge_orientation(cube, edge) == 1) {
			flipped_edges++;
		}
	}
	bool orientation_parity_error = flipped_edges%2 != 0;
	bool permutation_parity_error = even_corner_parity(cube) != even_edge_parity(cube);
	int middle = (cube.get_size()-1)/2;
	if (orientation_parity_error && permutation_parity_error) {
		return TwistSequence({
			Twist(90, Face::RIGHT, middle),
			Twist(90, Face::FRONT),
			Twist(90, Face::FRONT),
			Twist(90, Face::TOP),
			Twist(90, Face::TOP),
			Twist(90, Face::LEFT, middle),
			Twist(90, Face::FRONT),
			Twist(-90, Face::TOP),
			Twist(90, Face::RIGHT),
			Twist(-90, Face::TOP),
			Twist(90, Face::RIGHT, middle),
			Twist(90, Face::RIGHT, middle),
			Twist(90, Face::BACK),
			Twist(90, Face::BACK),
			Twist(-90, Face::RIGHT, middle),
			Twist(90, Face::BACK),
			Twist(90, Face::BACK),
			Twist(90, Face::RIGHT, middle),
			Twist(90, Face::RIGHT, middle),
		});
	}
	else if (orientation_parity_error) {
		return TwistSequence({
			Twist(90, Face::RIGHT, middle),
			Twist(90, Face::BACK),
			Twist(90, Face::TOP),
			Twist(90, Face::TOP),
			Twist(-90, Face::BACK),
			Twist(90, Face::RIGHT, middle),
			Twist(-90, Face::RIGHT),
			Twist(90, Face::BACK),
			Twist(90, Face::BACK),
			Twist(90, Face::LEFT, middle),
			Twist(-90, Face::LEFT),
			Twist(90, Face::BACK),
			Twist(90, Face::BACK),
			Twist(90, Face::RIGHT, middle),
			Twist(-90, Face::RIGHT),
			Twist(-90, Face::BACK),
			Twist(90, Face::BOTTOM),
			Twist(90, Face::BOTTOM),
			Twist(90, Face::BACK),
			Twist(90, Face::RIGHT, middle),
		});
	}
	else if (permutation_parity_error) {
		return TwistSequence({
			Twist(90, Face::RIGHT, middle),
			Twist(90, Face::TOP),
			Twist(90, Face::BOTTOM),
			Twist(90, Face::LEFT),
			Twist(90, Face::LEFT),
			Twist(90, Face::TOP),
			Twist(90, Face::BOTTOM),
			Twist(90, Face::FRONT, cube.get_size()-2),
			Twist(90, Face::FRONT, cube.get_size()-2),
			Twist(-90, Face::FRONT),
			Twist(-90, Face::FRONT),
			Twist(90, Face::RIGHT, middle),
			Twist(-90, Face::FRONT, cube.get_size()-2),
			Twist(-90, Face::FRONT, cube.get_size()-2),
		});
	}
	
	return TwistSequence();
}

std::vector<cube::Twist> ThreeCubeSolver::get_twists(const CubeState<cube::Cube>* state) {
	std::vector<cube::Twist> twists;

	auto curr_ptr = state;
	while (curr_ptr->parent != nullptr) {
		for (const auto& twist : curr_ptr->twist_seq.get()) {
			twists.push_back(cube::Twist(-twist.degrees, twist.face, twist.layer, twist.wide_turn));
		}
		curr_ptr = curr_ptr -> parent.get();	
	}
	
	return twists;	

}

std::vector<TwistSequence> ThreeCubeSolver::generate_twist_sequences(const std::unordered_set<cube::Face>& restricted_faces) {
	std::vector<TwistSequence> twist_sequences;
	for (const auto face : cube::ALL_FACES) {
		if (restricted_faces.count(face)) {
			twist_sequences.push_back(TwistSequence({
				cube::Twist(90, face),
				cube::Twist(90, face),
			}));	
		}
		else {
			for (const int deg : TwistUtils::DEGREES) {
				twist_sequences.push_back(TwistSequence({cube::Twist(deg, face)}));
			}		
		}
	}	

	return twist_sequences;
}

bool ThreeCubeSolver::load_lookup_table(std::unordered_map<std::vector<bool>, std::vector<cube::Twist>>& table, std::string file_path) {
	if (!boost::filesystem::exists(table_dir)) {
		boost::filesystem::create_directory(table_dir);
	}

	if (boost::filesystem::exists(file_path)) {
		std::ifstream in_stream(file_path);
		boost::archive::binary_iarchive loader(in_stream);
		loader >> table;

		return true;
	}

	return false;
}

void ThreeCubeSolver::save_lookup_table(const std::unordered_map<std::vector<bool>, std::vector<cube::Twist>>& table, std::string file_path) {
	std::ofstream out_stream(file_path);
	boost::archive::binary_oarchive saver(out_stream);
	saver << table;
}

void ThreeCubeSolver::build_encoding(std::vector<bool>& encoding, const uint8_t data, const int bits) {
	for (int i = 0; i < bits; i++) {
		bool bit = (data >> i) & 1;
		encoding.push_back(bit);
	}
}

std::vector<bool> ThreeCubeSolver::encode_g1(const cube::Cube& cube) {
	std::vector<bool> encoding;
	for (int edge = 0; edge < cube.get_edge_count(); edge++) {
		build_encoding(encoding, get_edge_orientation(cube, edge), 1);
	}
	
	return encoding;
}

std::vector<bool> ThreeCubeSolver::encode_g2(const cube::Cube& cube) {
	std::vector<bool> encoding;
	for (int corner = 0; corner < cube.get_corner_count(); corner++) {
		build_encoding(encoding, cube.get_corner_orientation(corner), 2);
	}
	for (int edge = 0; edge < cube.get_edge_count(); edge++) {
		build_encoding(encoding, edge_slices[get_edge_pos(cube, edge)]==1, 1);
	}
	
	return encoding;
}

std::vector<bool> ThreeCubeSolver::encode_g3(const cube::Cube& cube) {
	std::vector<bool> encoding;
	
	static std::unordered_map<int, int> corner_pair_groups = {
		{0, 0}, {2, 0}, 
		{3, 1}, {1, 1}, 
		{4, 2}, {6, 2},
		{7, 3}, {5, 3},
	};

	for (int corner = 0; corner < cube.get_corner_count(); corner++) {
		build_encoding(encoding, corner_pair_groups[cube.get_corner_pos(corner)], 2);
	}
	for (int edge = 0; edge < cube.get_edge_count(); edge++) {
		if (edge_slices[get_edge_pos(cube, edge)] != 1) {
			build_encoding(encoding, edge_slices[get_edge_pos(cube, edge)], 2);
		}
	}
	build_encoding(encoding, even_corner_parity(cube), 1);
	
	return encoding;
}

std::vector<bool> ThreeCubeSolver::encode_g4(const cube::Cube& cube) {
	std::vector<bool> encoding;
	for (int edge = 0; edge < cube.get_edge_count(); edge++) {
		build_encoding(encoding, get_edge_pos(cube, edge), 4);
	}
	for (int corner = 0; corner < cube.get_corner_count(); corner++) {
		build_encoding(encoding, cube.get_corner_pos(corner), 4);
	}

	return encoding;
}

std::unordered_map<std::vector<bool>, std::vector<cube::Twist>> ThreeCubeSolver::create_lookup_table(
			const std::function<std::vector<bool>(const cube::Cube&)> encoder, 
			const std::vector<TwistSequence> twist_sequences) {

		std::queue<std::shared_ptr<State>> open;
	       	open.push(std::make_shared<State>(cube::Cube(3)));
		LookupTable table;
		while (open.size() > 0) {
			auto& curr_state = open.front();
			for (const auto& twist_seq : twist_sequences) {
				cube::Cube child_cube(curr_state->cube);
				for (const auto& twist : twist_seq) {
					child_cube.rotate(twist);
				}

				auto child_ptr = std::make_shared<State>(curr_state, std::move(child_cube), twist_seq);
				auto child_encoding = encoder(child_cube);
				if (!table.count(child_encoding)) {
					table.insert(std::make_pair(child_encoding, get_twists(child_ptr.get())));
					open.push(child_ptr);
				}	
			}
			open.pop();
		}

		return table;
}

void ThreeCubeSolver::execute_partial_solution(const TwistSequence& twist_sequence, cube::Cube& cube) {
	notify_listeners(twist_sequence);
	for (const auto& twist : twist_sequence) {
		cube.rotate(twist);	
	}
}

void ThreeCubeSolver::solve(const cube::Cube& cube, const cube::CubeCenters& centers) {
	cube::Cube curr_state(cube);

	execute_partial_solution(orient_cube(centers), curr_state);
	execute_partial_solution(solve_parity(curr_state), curr_state);
	for (int stage = 0; stage < stage_count; stage++) {
		execute_partial_solution(tables[stage][encoders[stage](curr_state)], curr_state);
		std::cout << "Stage " << stage << " complete\n";
	}
}
