#include <deque>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include "three_cube_solver.h"
#include "cube.h"
#include "face.h"
#include "twist_utils.h"

using namespace ai;
ThreeCubeSolver::ThreeCubeSolver() {
	using namespace cube;
	std::string filenames[] = {
		"g1_table.bin",
		"g2_table.bin",
		"g3_table.bin",
		"g4_table.bin",
	};

	std::unordered_set<cube::Face> restricted_faces[] {
		{},
		{Face::TOP, Face::BOTTOM},	
		{Face::TOP, Face::BOTTOM, Face::FRONT, Face::BACK},
		{Face::TOP, Face::BOTTOM, Face::FRONT, Face::BACK, Face::LEFT, Face::RIGHT},
	};

	for (int stage = 0; stage < 4; stage++) {
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

bool ThreeCubeSolver::even_parity(const cube::Cube& cube) {
	int corner_pos[8];
	for (int i = 0; i < 8; i++) {
		corner_pos[i] = cube.get_corner_pos(i);	
	}

	int cycles = 0;
	int start_index = 0;
	while (start_index < 8) {
		if (corner_pos[start_index] < 8) {
			int cycle_index = start_index;
			while (corner_pos[cycle_index] < 8) {
				int& element = corner_pos[cycle_index];
				cycle_index = element;
				element += 8;
			}
			cycles++;	
		}
		start_index++;
	}

	return (8-cycles)%2 == 0 ? 1 : 0;

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

bool ThreeCubeSolver::load_lookup_table(std::unordered_map<std::vector<uint8_t>, std::vector<cube::Twist>>& table, std::string file_path) {
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

void ThreeCubeSolver::save_lookup_table(const std::unordered_map<std::vector<uint8_t>, std::vector<cube::Twist>>& table, std::string file_path) {
	std::ofstream out_stream(file_path);
	boost::archive::binary_oarchive saver(out_stream);
	saver << table;
}

std::vector<uint8_t> ThreeCubeSolver::encode_g1(const cube::Cube& cube) {
	std::vector<uint8_t> encoding;
	uint8_t buffer = 0;
	int buffer_index = 0;

	for (int i = 0; i < 12; i++) {
		buffer |= cube.get_edge_orientation(i) << buffer_index;
		buffer_index++;

		if (buffer_index == 8) {
			encoding.push_back(buffer);
			buffer = 0;
			buffer_index = 0;	
		}
	}
	encoding.push_back(buffer);

	return encoding;
}

std::vector<uint8_t> ThreeCubeSolver::encode_g2(const cube::Cube& cube) {
	std::vector<uint8_t> encoding;
	uint8_t buffer = 0;
	int buffer_index = 0;

	for (int i = 0; i < 8; i++) {
		buffer |= cube.get_corner_orientation(i) << buffer_index;
		buffer_index += 2;

		if (buffer_index == 8) {
			encoding.push_back(buffer);
			buffer = 0;
			buffer_index = 0;	
		}	
	}
	for (int i = 0; i < 12; i++) {
		buffer |= (edge_slices[cube.get_edge_pos(i)] == 1 ? 1 : 0) << buffer_index;
		buffer_index++;

		if (buffer_index == 8) {
			encoding.push_back(buffer);
			buffer = 0;
			buffer_index = 0;	
		}
	}
	encoding.push_back(buffer);

	return encoding;
}

std::vector<uint8_t> ThreeCubeSolver::encode_g3(const cube::Cube& cube) {
	std::vector<uint8_t> encoding;
	uint8_t buffer = 0;
	int buffer_index = 0;

	static std::unordered_map<int, int> corner_pair_groups = {
		{0, 0}, {2, 0}, 
		{3, 1}, {1, 1}, 
		{4, 2}, {6, 2},
		{7, 3}, {5, 3},
	};

	for (int i = 0; i < 8; i++) {
		buffer |= corner_pair_groups[cube.get_corner_pos(i)] << buffer_index;
		buffer_index += 2;
		
		if (buffer_index == 8) {
			encoding.push_back(buffer);
			buffer = 0;
			buffer_index = 0;	
		}	
	}

	for (int i = 0; i < 12; i++) {
		if (edge_slices[cube.get_edge_pos(i)] != 1) {
			buffer |= edge_slices[cube.get_edge_pos(i)] << buffer_index;
			buffer_index += 2;
			
			if (buffer_index == 8) {
				encoding.push_back(buffer);
				buffer = 0;
				buffer_index = 0;
			}
		}

		
	}
	encoding.push_back(even_parity(cube));

	return encoding;
}

std::vector<uint8_t> ThreeCubeSolver::encode_g4(const cube::Cube& cube) {
	std::vector<uint8_t> encoding;
	for (int i = 0; i < 12; i++) {
		encoding.push_back(cube.get_edge_pos(i));	
	}
	for (int i = 0; i < 8; i++) {
		encoding.push_back(cube.get_corner_pos(i));	
	}

	return encoding;
}

std::unordered_map<std::vector<uint8_t>, std::vector<cube::Twist>> ThreeCubeSolver::create_lookup_table(
			const std::function<std::vector<uint8_t>(const cube::Cube&)> encoder, 
			const std::vector<TwistSequence> twist_sequences) {

		std::deque<std::shared_ptr<State>> open {std::make_shared<State>(cube::Cube(3))};
		LookupTable table;
		while (open.size() > 0) {
			auto& curr_state = open.front();
			for (const TwistSequence& twist_seq : twist_sequences) {
				cube::Cube child_cube(curr_state->cube);
				for (const auto& twist : twist_seq) {
					child_cube.rotate(twist);
				}

				auto child_ptr = std::make_shared<State>(curr_state, std::move(child_cube), twist_seq);
				std::vector<uint8_t> child_encoding = encoder(child_cube);
				if (!table.count(child_encoding)) {
					table.insert(std::make_pair(child_encoding, get_twists(child_ptr.get())));
					open.push_back(child_ptr);
				}	
			}
			open.pop_front();
		}

		return table;
}

std::vector<cube::Twist> ThreeCubeSolver::solve(cube::Cube& cube) {
	std::vector<cube::Twist> moves;

	for (int stage = 0; stage < 4; stage++) {
		for (const auto& twist : tables[stage][encoders[stage](cube)]) {
			moves.push_back(twist);
			cube.rotate(twist);	
		}
		std::cout << "Stage " << stage << " complete\n";
	}
	
	return moves;
}
