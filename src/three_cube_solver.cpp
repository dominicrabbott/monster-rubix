#include "three_cube_solver.h"
#include "cube.h"
#include "cube_centers.h"
#include "combined_cube.h"
#include "face.h"
#include "twist_utils.h"
#include "search.h"
#include "hash.h"
#include <queue>
#include <cmath>
#include <algorithm>
#include <iostream>

using namespace ai;

ThreeCubeSolver::ThreeCubeSolver() : twist_mappings(create_twist_mappings()), table_dir("tables") {
	using namespace cube;
	std::array<std::unordered_set<cube::Face>, stage_count> restricted_faces = {{
		{},
		{Face::TOP, Face::BOTTOM},	
		{Face::TOP, Face::BOTTOM, Face::FRONT, Face::BACK},
		{Face::TOP, Face::BOTTOM, Face::FRONT, Face::BACK, Face::LEFT, Face::RIGHT},
	}};
	if (!boost::filesystem::exists(table_dir)) {
		boost::filesystem::create_directory(table_dir);
	}
	for (int stage = 0; stage < stage_count; stage++) {
		auto file_path = table_dir/filenames[stage];
		if (!boost::filesystem::exists(file_path)) {
			std::cout << "Generating table for stage " << stage << ". This may take some time."  << "\n";
			save_lookup_table(create_lookup_table(encoders[stage], generate_twist_sequences(restricted_faces[stage])), file_path.string());
			std::cout << "Table for stage " << stage << " generated\n";
		}
	}
}

ThreeCubeSolver::TwistEncodingMap ThreeCubeSolver::create_twist_mappings() {
		TwistEncodingMap twist_mappings;
		char encoding = 0;
		for (const auto face : cube::ALL_FACES) {
			for (const int degrees : TwistUtils::DEGREES) {
				encoding++;
				twist_mappings.insert(TwistEncodingMap::value_type(cube::Twist(degrees, face), encoding));
			}
		}

		return twist_mappings;

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

void ThreeCubeSolver::save_lookup_table(const ThreeCubeSolver::LookupTable& table, std::string file_path) {
	typedef std::pair<std::vector<char>, std::vector<cube::Twist>> TableEntry;
	std::vector<TableEntry> sorted_entries;
	for (const auto& entry : table) {
		sorted_entries.push_back(std::make_pair(convert_encoding(entry.first), entry.second));
	}
	auto entry_compare = [](const TableEntry& lhs, const TableEntry& rhs) {
		return lhs.first < rhs.first;
	};
	std::sort(sorted_entries.begin(), sorted_entries.end(), entry_compare);
	
	std::ofstream file(file_path, std::ofstream::binary);
	for (const auto& entry : sorted_entries) {
		file.write(&entry.first[0], entry.first.size());
		std::vector<char> twists_encoding;
		for (const auto& twist : entry.second) {
			twists_encoding.push_back(twist_mappings.left.at(twist));
		}
		while (twists_encoding.size() < twists_encoding_length) {
			twists_encoding.push_back(0);	
		}
		file.write(&twists_encoding[0], twists_encoding.size());
	}
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

std::vector<char> ThreeCubeSolver::convert_encoding(const std::vector<bool>& encoding) {
	uint8_t buffer = 0;
	uint8_t buffer_index = 0;
	std::vector<char> converted_encoding;
	for (const bool b : encoding) {
		buffer |= b << buffer_index;
		buffer_index++;
		if (buffer_index == 8) {
			converted_encoding.push_back(buffer);
			buffer = 0;
			buffer_index = 0;
		}
	}
	converted_encoding.push_back(buffer);
	while (converted_encoding.size() < cube_encoding_length) {
		converted_encoding.push_back(0);
	}
	
	return converted_encoding;
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

std::vector<cube::Twist> ThreeCubeSolver::find_table_entry(
		const std::vector<bool>& target_encoding,
		const boost::iostreams::mapped_file_source& table) {

	auto converted_encoding = convert_encoding(target_encoding);
	const char* lower_bound = table.begin();
	const char* upper_bound = table.end()-table_entry_length;
	while (1) {
		int table_entries_in_range = std::distance(lower_bound, upper_bound)/table_entry_length;
		auto pivot = lower_bound + (table_entries_in_range/2)*table_entry_length;
		if (std::equal(pivot, pivot+cube_encoding_length, &converted_encoding[0])) {
			std::vector<cube::Twist> solution;
			for (int i = cube_encoding_length; i < table_entry_length; i++) {
				if (pivot[i] == 0) {
					break;
				}
				solution.push_back(twist_mappings.right.at(pivot[i]));
			}

			return solution;
		}
		else if (std::lexicographical_compare(pivot, pivot+cube_encoding_length, 
					&converted_encoding[0], &converted_encoding[0]+cube_encoding_length)) {
			lower_bound = pivot+table_entry_length;
		}
		else {
			upper_bound = pivot-table_entry_length;
		}
	}
}

void ThreeCubeSolver::execute_partial_solution(const TwistSequence& twist_sequence, cube::CombinedCube& comb_cube) {
	notify_listeners(twist_sequence);
	for (const auto& twist : twist_sequence) {
		comb_cube.rotate(twist);	
	}
}

void ThreeCubeSolver::solve(const cube::CombinedCube& comb_cube) {
	cube::CombinedCube curr_state(comb_cube);

	execute_partial_solution(orient_cube(comb_cube.get_cube_centers()), curr_state);
	execute_partial_solution(solve_parity(curr_state.get_cube()), curr_state);

	for (int stage = 0; stage < stage_count; stage++) {
		auto table_path = table_dir/filenames[stage];
		boost::iostreams::mapped_file_source table(table_path.string());
		execute_partial_solution(find_table_entry(encoders[stage](comb_cube.get_cube()), table), curr_state);
		std::cout << "Stage " << stage << " complete\n";
	}
}
