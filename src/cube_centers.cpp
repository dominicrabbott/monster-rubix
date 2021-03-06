#include "cube_centers.h"
#include "twist.h"
#include <cmath>
#include <iostream>
#include <unordered_set>
#include <vector>
#include <algorithm>

using namespace cube;

CubeCenters::CubeCenters(const int size) : 
	CubeBase(size),
	center_size(std::pow(edge_width,2)),
	centers(std::make_unique<uint8_t[]>(center_size*cube::ALL_FACES.size())), 
	width_in_cartesian_space(size-1) {

	if (size%2 == 0) {
		solved_center_values = std::make_unique<cube::CubeCenters>(3);
	}
	for (int i = 0; i < center_size*face_count; i++) {
		centers[i] = i/center_size;
	}
}

CubeCenters::CubeCenters(const CubeCenters& cube) : 
	CubeBase(cube),
	center_size(cube.center_size),
	centers(copy_pieces(cube.centers.get(), center_size*face_count)),
	width_in_cartesian_space(cube.width_in_cartesian_space) {
		if (cube.solved_center_values != nullptr) {
			solved_center_values = std::make_unique<cube::CubeCenters>(*cube.solved_center_values);
		}
	}

CubeCenters& CubeCenters::operator=(const CubeCenters& cube) {
	assert(cube.size == size && "Cube classes can only be set to objects of the same size");
	
	std::copy(cube.centers.get(), cube.centers.get() + center_size*face_count, centers.get());
	if (cube.solved_center_values != nullptr) {
		solved_center_values = std::make_unique<cube::CubeCenters>(*cube.solved_center_values);
	}	

	return *this;
}

bool CubeCenters::operator==(const CubeCenters& cube) const {
	if (cube.size != size) {
		return false;	
	}
	return std::equal(cube.centers.get(), cube.centers.get() + center_size*face_count, centers.get());
}

void CubeCenters::transpose_center(const Face face) {
	//the index of the first piece of the center of the given piece
	int center_start = static_cast<int>(face)*center_size;

	//an in-place square matrix transposition is performed on the center pieces of the given face
	for (int i = 0; i < edge_width-1; i++) {
		for (int j = i+1; j < edge_width; j++) {
			std::swap(centers[center_start + i*edge_width + j], centers[center_start + j*edge_width + i]);
		}	
	}
}

void CubeCenters::reverse_center_rows(const Face face) {
	//the index of the first piece of the center of the given piece
	int center_start = static_cast<int>(face)*center_size;

	//the rows of the center of the given face are each reversed
	for (int i = 0; i < edge_width; i++) {
		int row_start = center_start + i*edge_width;
		std::reverse(centers.get()+row_start, centers.get()+row_start+edge_width);
	}
}

void CubeCenters::rotate_face(const Face face, const int degrees) {
	//account for the mirroring of rotations for opposing faces
	int adjusted_degrees = degrees;
	if (face == Face::BACK || face == Face::LEFT || face == Face::TOP) {
		adjusted_degrees *= -1;	
	}

	//rotate the center of the specified face
	if (adjusted_degrees == 90) {
		reverse_center_rows(face);
		transpose_center(face);
	}
	else {
		transpose_center(face);
		reverse_center_rows(face);
	}
}

void CubeCenters::rotate_slice(const Face slice_face, const int layer, const int degrees) {
	///map specifies the the centers that exist in each slice
	//
	//the indecies of the center pieces in the slice are listed as follows:
	//   | 0 | 
	//----------
	// 3 |   | 1
	//----------
	//   | 2 | 
	static std::unordered_map<Face, std::array<Face,4>> slice_centers = {
		{Face::LEFT, {Face::TOP, Face::FRONT, Face::BOTTOM, Face::BACK}},
		{Face::TOP, {Face::BACK, Face::RIGHT, Face::FRONT, Face::LEFT}},
		{Face::BACK, {Face::TOP, Face::LEFT, Face::BOTTOM, Face::RIGHT}},
		{Face::BOTTOM, {Face::FRONT, Face::RIGHT, Face::BACK, Face::LEFT}},
		{Face::FRONT, {Face::TOP, Face::RIGHT, Face::BOTTOM, Face::LEFT}},
		{Face::RIGHT, {Face::TOP, Face::BACK, Face::BOTTOM, Face::FRONT}},
	};

	auto& center_shifts = slice_centers[slice_face];
	//the centers are shifted with n 4-cycles, where n = edge_width. To shift the pieces,
	//the pieces moved in the 4-cycle are found in 3D cartesian space. The coordinates of the
	//pieces are then converted to an index in the 'centers' array
	for (int cycle = 1; cycle <= edge_width; cycle++) {
		std::array<int, 4> piece_shifts;
		int previous_cycle_coordinate = -1;
		for (int i = 0; i < 4; i++) {
			//stores the coordinates of the piece being shifted within the
			//face it is being shifted to
			std::vector<int> piece_coords;
			
			Face current_face = center_shifts[i];
			for (int coord = 0; coord < 3; coord++) {
				//one coordinate is fixed by the slice that is being rotated
				if (face_coords[slice_face][0] == coord) {
					//account for the fact that layers are counted from opposite sides of the cube on opposing faces
					int adjusted_layer = face_coords[slice_face][1]==0 ? layer : edge_width-layer+1;
					piece_coords.push_back(adjusted_layer);
				}
				//the other coordinate is determined by which 4-cycle is currently being performed.
				//
				//To find this last coordinate, one needs to observe that when a piece is shifted, it
				//goes to a coordinate who's manhattan distance is (edge_width+1), or (width_in_cartesian_space), from its original position.
				//In order to find the last coordinate, represented by 'c', the following equation based on manhattan distance is 
				//solved for 'c'.
				//
				// width_in_cartesian_space = |c-previous_face_coord| + |current_face_coord - previous_cycle_coordinate|
				// 
				// 'previous_face_coord' is the fixed coordinate of the face the piece is being shifted from
				// 'current_face_coord' is the fixed coordinate of the face the piece is being shifted to
				// 'previous_cycle_coordinate' is the coordinate of the piece before it was shifted that was determined by which 4-cycle is being performed
				else if (face_coords[current_face][0] != coord) {
					if (previous_cycle_coordinate == -1) {
						piece_coords.push_back(cycle);
						previous_cycle_coordinate = cycle;
					}
					else {
						int partial_solution = width_in_cartesian_space-std::abs(face_coords[current_face][1]-previous_cycle_coordinate);
						int previous_face_coord = face_coords[center_shifts[i-1]][1];

						int solution_1 = partial_solution+previous_face_coord;
						int solution_2 = -partial_solution+previous_face_coord;

						if (solution_1 > 0 && solution_1 <= edge_width) {
							piece_coords.push_back(solution_1);	
						}
						else {
							piece_coords.push_back(solution_2);
						}
						previous_cycle_coordinate = piece_coords.back();
					}
				}
			}

			//the coordinates determined above are converted to a piece index
			piece_shifts[i] = center_size*static_cast<int>(current_face) + (piece_coords[1]-1)*edge_width + piece_coords[0]-1;
		}	
		shift_pieces(centers.get(), piece_shifts, degrees);
	}
}

int CubeCenters::get_center_pos(const Coords coords) const {
	std::array<Face,3> origin_faces = {Face::LEFT, Face::BOTTOM, Face::BACK};
	Face face;
	std::vector<int> face_coords;
	for (int i = 0; i < coords.size(); i++) {
		if (coords[i] == 0) {
			face = origin_faces[i];	
		}
		else if (coords[i] == width_in_cartesian_space) {
			face = cube::OPPOSING_FACES.at(origin_faces[i]);	
		}
		else {
			face_coords.push_back(coords[i]);	
		}
	}
	
	return centers[static_cast<int>(face)*center_size + (face_coords[1]-1)*edge_width + face_coords[0]-1];
}

int CubeCenters::get_solved_center_value(const Face face) const {
	if (solved_center_values == nullptr) {
		return centers[static_cast<int>(face)*center_size + (center_size/2)];
	}
	else {
		return solved_center_values->get_center_pos(static_cast<int>(face));
	}
}

void CubeCenters::rotate(const Twist& twist) {
	if (solved_center_values != nullptr && twist.layer == size-1 && twist.wide_turn) {
		solved_center_values -> rotate(Twist(twist.degrees, twist.face, 1, false));	
	}
	for (int i = twist.layer; i >= (twist.wide_turn ? 0 : twist.layer); i--) {
		if (i == 0) {
			rotate_face(twist.face, twist.degrees);	
		}
		else if (i == size - 1) {
			rotate(Twist(-twist.degrees, OPPOSING_FACES.at(twist.face)));
		}
		else {
			rotate_slice(twist.face, i, twist.degrees);
		}	
	}
}	
