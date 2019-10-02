#include <cmath>
#include <iostream>
#include <unordered_set>
#include <cassert>
#include <boost/functional/hash.hpp>

#include "cube.h"
#include "twist.h"

using namespace cube;

std::unordered_map<Face, int> Cube::face_numbers = {
	{Face::LEFT, 0},
	{Face::TOP, 1},
	{Face::BACK, 2},
	{Face::BOTTOM, 3},
	{Face::FRONT, 4},
	{Face::RIGHT, 5},

};

Cube::Cube(const int size) : 
	size(size),
	edge_width(size-2),
	center_size(std::pow(size-2,2)),
	centers(std::make_unique<unsigned char[]>(center_size*6)), 
	edges(std::make_unique<unsigned char[]>(edge_width*12)),
	corners(std::make_unique<unsigned char[]>(8)) {

	assert(size <= 8 && "Cube class can only represent 8x8x8 cubes or smaller");

	for (int i = 0; i < center_size*6; i++) {
		centers[i] = i;
	}
	for (int i = 0; i < edge_width*12; i++) {
		edges[i] = i;
	}
	for (int i = 0; i < 8; i++) {
		corners[i] = i;
	}
}

Cube::Cube(const Cube& cube) : 
	size(cube.size),
	edge_width(cube.edge_width),
	center_size(cube.center_size),
	centers(copy_pieces(cube.centers.get(), center_size*6)), 
	edges(copy_pieces(cube.edges.get(), edge_width*12)), 
	corners(copy_pieces(cube.corners.get(), 8)) {}

Cube& Cube::operator=(const Cube& cube) {
	assert(cube.size == size && "Cube classes can only be set to objects of the same size");

	for (int i = 0; i < center_size*6; i++) {
		centers[i] = cube.centers[i];	
	}
	for (int i = 0; i < edge_width*12; i++) {
		edges[i] = cube.edges[i];	
	}
	for (int i = 0; i < 8; i++) {
		corners[i] = cube.corners[i];	
	}

	return *this;
}

bool Cube::operator==(const Cube& cube) const {
	for (int i = 0; i < center_size*6; i++) {
		if (cube.centers[i] != centers[i]) {
			return false;	
		}	
	}

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

std::unique_ptr<unsigned char[]> Cube::copy_pieces(const unsigned char* array, const int size) {
	std::unique_ptr<unsigned char[]> result = std::make_unique<unsigned char[]>(size);

	for (int i = 0; i < size; i++) {
		result[i] = array[i];	
	}

	return result;
}

void Cube::transpose_center(const Face face) {
	//the index of the first piece of the center of the given piece
	int center_start = face_numbers[face]*center_size;

	//an in-place square matrix transposition is performed on the center pieces of the given face
	for (int i = 0; i < edge_width-1; i++) {
		for (int j = i+1; j < edge_width; j++) {
			std::swap(centers[center_start + i*edge_width + j], centers[center_start + j*edge_width + i]);
		}	
	}
}

void Cube::reverse_center_rows(const Face face) {
	//the index of the first piece of the center of the given piece
	int center_start = face_numbers[face]*center_size;

	//the rows of the center of the given face are each reversed
	for (int i = 0; i < edge_width; i++) {
		for (int j = 0; j < edge_width/2; j++) {
			int row_start = center_start + i*edge_width;
			std::swap(centers[row_start+j], centers[row_start+edge_width-j-1]);	
		}
	}
}

void Cube::shift_pieces(unsigned char* pieces, int* indecies, int degrees, int offset) {
	int shifts = degrees == 90 ? 1 : 3;
	
	int temp = pieces[indecies[0]+offset];
	for (int i = 4-shifts; i != 0; i = (i-shifts+4)%4) {
		pieces[indecies[(i+shifts)%4]+offset] = pieces[indecies[i]+offset];	
	}
	pieces[indecies[shifts]+offset] = temp;
}

void Cube::rotate_face(const Face face, const int degrees) {
	//array specifies the indicies of the corner pieces that exist in each face.
	//
	//the corners of a face are numbered as follows:
	// 0 |  | 1
	//---------
	//   |  | 
	//---------
	// 3 |  | 2
	int face_corners[6][4] = {
		{4,7,3,0}, //LEFT
		{4,5,6,7}, //TOP
		{5,4,0,1}, //BACK
		{3,2,1,0}, //BOTTOM
		{7,6,2,3}, //FRONT
		{6,5,1,2}, //RIGHT
	};

	//array specifies the indicies of the edge pieces that exist in each face
	//
	//the edges of a face are numbered as follows
	//   | 0 | 
	//----------
	// 3 |   | 1
	//----------
	//   | 2 | 
	int face_edges[6][4] = {
		{11,7,3,4}, //LEFT
		{8,9,10,11},//TOP
		{8,4,0,5},  //BACK
		{2,1,0,3},  //BOTTOM
		{10,6,2,7}, //FRONT
		{9,5,1,6},  //RIGHT
	};

	//rotate the center of the specified face
	if (degrees == 90) {
		reverse_center_rows(face);
		transpose_center(face);
	}
	else {
		transpose_center(face);
		reverse_center_rows(face);
	}

	int rotations = degrees == 90 ? 1 : 3;	
	
	auto& corner_shifts = face_corners[face_numbers[face]];
	//manipulate the corners of the face to be rotated
	if ((face != cube::Face::TOP) && (face != cube::Face::BOTTOM)) {
		for (int i = 0; i < 4; i++) {
			//Rotate the corners
			//
			//In a 90 degree rotation, if a corner is moved from the U face to the D face
			//or from the D face to the U face, the corner is rotated clockwise. Otherwise, 
			//the corner is rotated counter-clockwise
			int rotation;
			if ((corner_shifts[(i+rotations)%4] < 4 && corner_shifts[i] < 4) || 
					(corner_shifts[(i+rotations)%4] >= 4 && corner_shifts[i] >= 4)) {
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
	shift_pieces(corners.get(), corner_shifts, degrees, 0);
	
	//manipulate the edges of the face to be rotated
	auto& edge_shifts = face_edges[face_numbers[face]];
	for (int i = 0; i < edge_width; i++) {
		//the orientation of each edge involved in the rotation is flipped
		for (int j = 0; j < 4; j++) {
			flip_edge(edge_shifts[j]*edge_width + i);	
		}
		
		//The edges are shifted among the edge positions of the 
		//face as specified in the face_edges array
		shift_pieces(edges.get(), edge_shifts, degrees, i);
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
	int slice_edges[6][4] = {
		{8,10,2,0}, //LEFT  
		{4,5,6,7},  //TOP
		{9,11,3,1}, //BACK
		{7,6,5,4},  //BOTTOM
		{11,9,1,3}, //FRONT
		{10,8,0,2}, //RIGHT
	};

	///array specifies the indicies of the centers that exist in each slice
	//
	//the indecies of the center pieces in the slice are numbered as follows:
	//   | 0 | 
	//----------
	// 3 |   | 1
	//----------
	//   | 2 | 
	int slice_centers[6][4] = {
		{1,4,3,2}, //LEFT  
		{2,5,4,0}, //TOP
		{1,0,3,5}, //BACK
		{4,5,2,0}, //BOTTOM
		{1,5,3,0}, //FRONT
		{1,2,3,4}, //RIGHT
	};

	//the number of 90 degree rotations to perform on the cube
	int rotations = degrees == 90 ? 1 : 3;

	//determine the inner layer of the cube to be rotated. The inner layers are numbered starting
	//from the Front, Left, and bottom faces and going to the opposite face
	int inner_layer;
	if (face == Face::FRONT || face == Face::LEFT || face == Face::BOTTOM) {
		inner_layer = layer-1;	
	}
	else {
		inner_layer = size-layer-2;	
	}
	
	//manipulate the edges in the slice being rotated
	
	auto& edge_shifts = slice_edges[face_numbers[face]];
	//the orientation of each edge involved in the rotation is flipped
	for (int i = 0; i < 4; i++) {
		flip_edge(edge_shifts[i]+inner_layer);
	}

	//the edges are rotated
	shift_pieces(edges.get(), edge_shifts, degrees, inner_layer);

	//move the center pieces in the slice being rotated
	auto& center_shifts = slice_centers[face_numbers[face]];
	for (int i = 0; i < edge_width; i++) {
		int center_index;
		if (face == Face::TOP || face == Face::BOTTOM) {
			center_index = i + inner_layer;	
		}
		else {
			center_index = (i*edge_width)+inner_layer;	
		}

		shift_pieces(centers.get(), center_shifts, degrees, center_index);
	}
}

void Cube::rotate(const Twist& twist) {
	static std::unordered_map<Face, Face> opposing_faces = {
		{Face::FRONT, Face::BACK},
		{Face::BACK, Face::FRONT},
		{Face::RIGHT, Face::LEFT},
		{Face::LEFT, Face::RIGHT},
		{Face::TOP, Face::BOTTOM},
		{Face::BOTTOM, Face::TOP},
	};

	for (int i = twist.layer; i >= (twist.wide_turn ? 0 : twist.layer); i--) {
		if (i == 0) {
			rotate_face(twist.face, twist.degrees);	
		}
		else if (i == size - 1) {
			rotate(Twist(-twist.degrees, opposing_faces[twist.face]));
		}
		else {
			rotate_slice(twist.face, twist.layer, twist.degrees);	
		}	
	}	
}

size_t std::hash<Cube>::operator()(const cube::Cube& cube) const {
	size_t seed = 0;
	boost::hash_range(seed, cube.centers.get(), cube.centers.get()+(cube.center_size*6));
	boost::hash_range(seed, cube.edges.get(), cube.edges.get()+(cube.edge_width*12));
	boost::hash_range(seed, cube.corners.get(), cube.corners.get()+8);

	return seed;
}
