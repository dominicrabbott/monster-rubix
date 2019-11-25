#include <cmath>
#include <iostream>
#include <unordered_set>

#include "cube_centers.h"
#include "twist.h"

int main();

using namespace cube;

CubeCenters::CubeCenters(const int size) : 
	CubeBase(size),
	center_size(std::pow(size-2,2)),
	centers(std::make_unique<uint8_t[]>(center_size*6)) {

	for (int i = 0; i < center_size*6; i++) {
		centers[i] = i/center_size;
	}
}

CubeCenters::CubeCenters(const CubeCenters& cube) : 
	CubeBase(cube),
	center_size(cube.center_size),
	centers(copy_pieces(cube.centers.get(), center_size*6)) {}

CubeCenters& CubeCenters::operator=(const CubeCenters& cube) {
	assert(cube.size == size && "Cube classes can only be set to objects of the same size");

	for (int i = 0; i < center_size*6; i++) {
		centers[i] = cube.centers[i];	
	}

	return *this;
}

bool CubeCenters::operator==(const CubeCenters& cube) const {
	for (int i = 0; i < center_size*6; i++) {
		if (cube.centers[i] != centers[i]) {
			return false;	
		}	
	}
	
	return true;
}

std::unique_ptr<uint8_t[]> CubeCenters::copy_pieces(const uint8_t* array, const int size) {
	std::unique_ptr<uint8_t[]> result = std::make_unique<uint8_t[]>(size);

	for (int i = 0; i < size; i++) {
		result[i] = array[i];	
	}

	return result;
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
		for (int j = 0; j < edge_width/2; j++) {
			int row_start = center_start + i*edge_width;
			std::swap(centers[row_start+j], centers[row_start+edge_width-j-1]);	
		}
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
			std::array<int, 2> piece_coords;
			
			Face current_face = center_shifts[i];
			
			int piece_coords_index = 0;
			for (int coord = 0; coord < 3; coord++) {
				//one coordinate is fixed by the slice that is being rotated
				if (face_coords[slice_face][0] == coord) {
					//account for the fact that layers are counted from opposite sides of the cube on opposing faces
					int adjusted_layer = face_coords[slice_face][1]==0 ? layer : edge_width-layer+1;

					piece_coords[piece_coords_index] = adjusted_layer;
					piece_coords_index++;
				}
				//the other coordinate is determined by which 4-cycle is currently being performed.
				//
				//To find this last coordinate, one needs to observe that when a piece is shifted, it
				//goes to a coordinate who's manhattan distance is (edge_width+1) from its original position.
				//In order to find the last coordinate, represented by 'c', the following equation based on manhattan distance is 
				//solved for 'c'.
				//
				// edge_width+1 = |c-previous_face_coord| + |current_face_coord - previous_cycle_coordinate|
				// 
				// 'previous_face_coord' is the fixed coordinate of the face the piece is being shifted from
				// 'current_face_coord' is the fixed coordinate of the face the piece is being shifted to
				// 'previous_cycle_coordinate' is the coordinate of the piece before it was shifted that was determined by which 4-cycle is being performed
				else if (face_coords[current_face][0] != coord) {
					if (previous_cycle_coordinate == -1) {
						piece_coords[piece_coords_index] = cycle;	
						previous_cycle_coordinate = cycle;
					}
					else {
						int partial_solution = edge_width+1-std::abs(face_coords[current_face][1]-previous_cycle_coordinate);
						int previous_face_coord = face_coords[center_shifts[i-1]][1];

						int solution_1 = partial_solution+previous_face_coord;
						int solution_2 = -partial_solution+previous_face_coord;

						if (solution_1 > 0 && solution_1 <= edge_width) {
							piece_coords[piece_coords_index] = solution_1;	
						}
						else {
							piece_coords[piece_coords_index] = solution_2;
						}

						previous_cycle_coordinate = piece_coords[piece_coords_index];
					}
					piece_coords_index++;
				}
			}

			//the coordinates determined above are converted to a piece index
			piece_shifts[i] = std::pow(edge_width,2)*static_cast<int>(current_face) + (piece_coords[1]-1)*edge_width + piece_coords[0]-1;
		}	
		shift_pieces(centers.get(), piece_shifts, degrees);
	}
}

int CubeCenters::get_center_pos(const Coords coords) const {
	int piece = 0;

	int face_coord_index = 0;
	for (int i = 0; i < 3; i++) {
		if (coords[i] == 0 || coords[i] == edge_width+1) {
			if (i == 0) {
				piece += center_size*(coords[i]==0 ? static_cast<int>(Face::LEFT) : static_cast<int>(Face::RIGHT));
			}
			else if (i == 1) {
				piece += center_size*(coords[i]==0 ? static_cast<int>(Face::BOTTOM) : static_cast<int>(Face::TOP));
			}
			else {
				piece += center_size*(coords[i]==0 ? static_cast<int>(Face::BACK) : static_cast<int>(Face::FRONT));
			}	
		}
		else {
			piece += coords[i]*(face_coord_index*edge_width);
			face_coord_index++;
		}	
	}

	return piece;
}

void CubeCenters::rotate(const Twist& twist) {
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
