#include <cmath>
#include <iostream>
#include <unordered_set>

#include "cube.h"
#include "twist.h"

using namespace cube;

Cube::Cube(const int size) : 
	CubeBase(size),
	edges(std::make_unique<uint8_t[]>(edge_width*12)),
	corners(std::make_unique<uint8_t[]>(8)) {

	for (int i = 0; i < edge_width*12; i++) {
		edges[i] = i/edge_width;
	}
	for (int i = 0; i < 8; i++) {
		corners[i] = i;
	}
}

Cube::Cube(const Cube& cube) : 
	CubeBase(cube),
	edges(copy_pieces(cube.edges.get(), edge_width*12)), 
	corners(copy_pieces(cube.corners.get(), 8)) {}

Cube& Cube::operator=(const Cube& cube) {
	assert(cube.size == size && "Cube classes can only be set to objects of the same size");

	for (int i = 0; i < edge_width*12; i++) {
		edges[i] = cube.edges[i];	
	}
	for (int i = 0; i < 8; i++) {
		corners[i] = cube.corners[i];	
	}

	return *this;
}

bool Cube::operator==(const Cube& cube) const {
	for (int i = 0; i < edge_width*12; i++) {
		if (cube.edges[i] != edges[i]) {
			return false;	
		}	
	}

	for (int i = 0; i < 8; i++) {
		if (cube.corners[i] != corners[i]) {
			return false;
		}		
	}

	return true;
}

std::unique_ptr<uint8_t[]> Cube::copy_pieces(const uint8_t* array, const int size) {
	std::unique_ptr<uint8_t[]> result = std::make_unique<uint8_t[]>(size);

	for (int i = 0; i < size; i++) {
		result[i] = array[i];	
	}

	return result;
}

void Cube::rotate_face(const Face face, const int degrees) {
	//map specifies the indicies of the corner pieces that exist in each face.
	//
	//the corners of a face are numbered as follows:
	// 0 |  | 1
	//---------
	//   |  | 
	//---------
	// 3 |  | 2
	static std::unordered_map<Face, std::array<int, 4>> face_corners = {
		{Face::LEFT, {4,7,3,0}},
		{Face::TOP, {4,5,6,7}},
		{Face::BACK, {5,4,0,1}},
		{Face::BOTTOM, {3,2,1,0}},
		{Face::FRONT, {7,6,2,3}},
		{Face::RIGHT, {6,5,1,2}},
	};

	//array specifies the indicies of the edge pieces that exist in each face
	//
	//the edges of a face are numbered as follows
	//   | 0 | 
	//----------
	// 3 |   | 1
	//----------
	//   | 2 | 
	static std::unordered_map<Face, std::array<int, 4>> face_edges = {
		{Face::LEFT, {11,7,3,4}},
		{Face::TOP, {8,9,10,11}},
		{Face::BACK, {8,4,0,5}},
		{Face::BOTTOM, {2,1,0,3}},
		{Face::FRONT, {10,6,2,7}},
		{Face::RIGHT, {9,5,1,6}},
	};

	//array specifies whether the pieces in each edge need to be shifted
	//in reverse order
	static std::unordered_map<Face, std::array<bool, 4>> reversed_shift_order = {
		{Face::LEFT, {0,1,1,0}},
		{Face::TOP, {0,0,1,1}},
		{Face::BACK, {0,0,1,1}},
		{Face::BOTTOM, {0,1,1,0}},
		{Face::FRONT, {0,1,1,0}},
		{Face::RIGHT, {0,0,1,1}},
	};

	//the corners that lie in the left face
	static std::unordered_set<int> left_corners = {0,3,4,7};

	//the corners that lie in the right face
	static std::unordered_set<int> right_corners = {1,2,5,6};

	int rotations = degrees == 90 ? 1 : 3;	
	
	auto& corner_shifts = face_corners[face];
	//manipulate the corners of the face to be rotated
	if ((face != cube::Face::RIGHT) && (face != cube::Face::LEFT)) {
		for (int i = 0; i < 4; i++) {
			//Rotate the corners
			//
			//In a 90 degree rotation of a face other than the right or left faces, if a 
			//corner is moved from the R face to the L face or from the L face to the R 
			//face, the corner is rotated clockwise. Otherwise, the corner is rotated 
			//counter-clockwise
			int rotation;
			if ((right_corners.count(corner_shifts[(i+rotations)%4]) && right_corners.count(corner_shifts[i])) || 
					(left_corners.count(corner_shifts[(i+rotations)%4]) && left_corners.count(corner_shifts[i]))) {
				rotation = degrees == 90 ? 1 : 2;
			}
			else {
				rotation = degrees == 90 ? 2 : 1;
			}
			rotate_corner(corner_shifts[i], rotation);
		}
	}

	//the corners are shifted among the 
	//corner positions of the face as specified in the face_corners array
	shift_pieces(corners.get(), corner_shifts, degrees);
	
	//manipulate the edges of the face to be rotated
	auto& edge_shifts = face_edges[face];
	for (int i = 0; i < edge_width; i++) {
		std::array<int, 4> current_shifts;
		for (int j = 0; j < 4; j++) {
			if (reversed_shift_order[face][j]) {
				current_shifts[j] = edge_shifts[j]*edge_width + edge_width - 1 - i; 
			}
			else {
				current_shifts[j] = edge_shifts[j]*edge_width + i;
			}
		}	
		
		//The edges are shifted among the edge positions of the 
		//face as specified in the face_edges array
		shift_pieces(edges.get(), current_shifts, degrees);
		
		//the orientation of each edge involved in the rotation is flipped
		for (int j = 0; j < 4; j++) {
			flip_edge(current_shifts[j], face);	
		}
	}
}

void Cube::rotate_slice(const Face face, const int layer, const int degrees) {
	//array specifies the indicies of the edge pieces that exist in each slice
	//
	//the indecies of the edge pieces in the slice are numbered as follows:
	// 0 |  | 1
	//---------
	//   |  | 
	//---------
	// 3 |  | 2
	static std::unordered_map<Face, std::array<int, 4>> slice_edges = {
		{Face::LEFT, {8,10,2,0}},
		{Face::TOP, {4,5,6,7}},
		{Face::BACK, {9,11,3,1}},
		{Face::BOTTOM, {7,6,5,4}},
		{Face::FRONT, {11,9,1,3}},
		{Face::RIGHT, {10,8,0,2}},
	};

	//the number of 90 degree rotations to perform on the cube
	int rotations = degrees == 90 ? 1 : 3;

	//determine the inner layer of the cube to be rotated. The inner layers are numbered starting
	//from the Back , Left, and bottom faces and going to the opposite face
	int inner_layer;
	if (face == Face::BACK || face == Face::LEFT || face == Face::BOTTOM) {
		inner_layer = layer-1;	
	}
	else {
		inner_layer = size-layer-2;	
	}
	
	//manipulate the edges in the slice being rotated
	std::array<int, 4> edge_shifts;
	for (int i = 0; i < 4; i++) {
		edge_shifts[i] = slice_edges[face][i]*edge_width + inner_layer;	
	}
	//flip the orientation of the edges in the slice
	for (int i = 0; i < 4; i++) {
		flip_edge(edge_shifts[i]);
	}
	
	//the edges are moved 
	shift_pieces(edges.get(), edge_shifts, degrees);
}

void Cube::rotate(const Twist& twist) {
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
