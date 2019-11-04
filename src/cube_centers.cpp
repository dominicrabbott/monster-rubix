#include <cmath>
#include <iostream>
#include <unordered_set>

#include "cube_centers.h"
#include "twist.h"

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

	//A slice rotation either slices a face horizontally or vertically. This map associates
	//faces with the slice rotation that slices that face vertically
	static std::unordered_map<Face, Face> vertical_slices = {
		{Face::FRONT, Face::LEFT},
		{Face::BACK, Face::LEFT},
		{Face::RIGHT, Face::BACK},
		{Face::LEFT, Face::BACK},
		{Face::TOP, Face::BACK},
		{Face::BOTTOM, Face::BACK},
	};

	auto& center_shifts = slice_centers[face];
	for (int cycle = 0; cycle < edge_width; cycle++) {
		std::array<int, 4> piece_shifts;
		for (int i = 0; i < 4; i++) {
			int x_face_coord;
			int y_face_coord;
			if (vertical_slices[center_shifts[i]] == vertical_slices[center_shifts[(i+1)%4]]) {
				x_face_coord = layer-1;
				y_face_coord = cycle;
			}
			else {
				x_face_coord = cycle;
				y_face_coord = layer-1;
			}

			piece_shifts[(i+1)%4] = static_cast<int>(center_shifts[(i+1)%4])*center_size + edge_width*y_face_coord + x_face_coord;
		}
		shift_pieces(centers.get(), piece_shifts, degrees);	
	}
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
			rotate_slice(twist.face, twist.layer, twist.degrees);	
		}	
	}	
}
