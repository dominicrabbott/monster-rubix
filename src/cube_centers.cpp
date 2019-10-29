#include <cmath>
#include <iostream>
#include <unordered_set>

#include "cube_centers.h"
#include "twist.h"

using namespace cube;

CubeCenters::CubeCenters(const int size) : 
	CubeBase(size),
	center_size(std::pow(size-2,2)),
	centers(std::make_unique<unsigned char[]>(center_size*6)) {

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

std::unique_ptr<unsigned char[]> CubeCenters::copy_pieces(const unsigned char* array, const int size) {
	std::unique_ptr<unsigned char[]> result = std::make_unique<unsigned char[]>(size);

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
	//rotate the center of the specified face
	if (degrees == 90) {
		reverse_center_rows(face);
		transpose_center(face);
	}
	else {
		transpose_center(face);
		reverse_center_rows(face);
	}
}

void CubeCenters::rotate_slice(const Face face, const int layer, const int degrees) {
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
	
	//move the center pieces in the slice being rotated
	auto& center_shifts = slice_centers[static_cast<int>(face)];
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

void CubeCenters::rotate(const Twist& twist) {
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
