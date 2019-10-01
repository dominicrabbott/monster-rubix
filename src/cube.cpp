#include <cmath>
#include <iostream>
#include <unordered_set>
#include <cassert>

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

std::unordered_map<Face, Face> Cube::opposing_faces = {
	{Face::FRONT, Face::BACK},
	{Face::BACK, Face::FRONT},
	{Face::RIGHT, Face::LEFT},
	{Face::LEFT, Face::RIGHT},
	{Face::TOP, Face::BOTTOM},
	{Face::BOTTOM, Face::TOP},
};


Cube::Cube(const int size) : 
			size(size), 
			centers(std::make_unique<unsigned char[]>(std::pow(size-2,2)*6)), 
			edges(std::make_unique<unsigned char[]>((size-2)*12)),
			corners(std::make_unique<unsigned char[]>(8)) {
	assert(size <= 8 && "Cube class can only represent 8x8x8 cubes or smaller");

	for (int i = 0; i < std::pow(size-2,2)*6; i++) {
		centers[i] = i;
	}
	for (int i = 0; i < 12*(size-2); i++) {
		edges[i] = i;
	}
	for (int i = 0; i < 8; i++) {
		corners[i] = i;
	}
}

Cube::Cube(const Cube& cube) : 
	size(cube.size), 
	centers(copy_pieces(cube.centers.get(), std::pow(size-2,2)*6)), 
	edges(copy_pieces(cube.edges.get(), 12*(size-2))), 
	corners(copy_pieces(cube.corners.get(), 8)) {}

Cube& Cube::operator=(const Cube& cube) {
	int center_count = std::pow(cube.size-2,2)*6;
	int edge_count = 12*(cube.size-2);
	int corner_count = 8;

	if (cube.size != size) {
		centers = std::make_unique<unsigned char[]>(center_count);
		edges = std::make_unique<unsigned char[]>(edge_count);
	}

	for (int i = 0; i < center_count; i++) {
		centers[i] = cube.centers[i];	
	}
	for (int i = 0; i < edge_count; i++) {
		edges[i] = cube.edges[i];	
	}
	for (int i = 0; i < corner_count; i++) {
		corners[i] = cube.corners[i];	
	}

	return *this;
}

std::unique_ptr<unsigned char[]> Cube::copy_pieces(const unsigned char* array, const int size) {
	std::unique_ptr<unsigned char[]> result = std::make_unique<unsigned char[]>(size);

	for (int i = 0; i < size; i++) {
		result[i] = array[i];	
	}

	return result;
}

void Cube::rotate_corner(const int corner) {
	int orientation = (get_corner_orientation(corner) + 1) % 3;
	corners[corner] &= 0xE7;
	corners[corner] |= orientation << 3;
}

void Cube::transpose_center(const Face face) {
	int center_width = size - 2;
	int center_start = face_numbers[face]*std::pow(center_width,2);

	for (int i = 0; i < center_width-1; i++) {
		for (int j = i+1; j < center_width; j++) {
			std::swap(centers[center_start + i*center_width + j], centers[center_start + j*center_width + i]);
		}	
	}
}

void Cube::reverse_center_rows(const Face face) {
	int center_width = size - 2;
	int center_start = face_numbers[face]*std::pow(center_width,2);

	for (int i = 0; i < center_width; i++) {
		for (int j = 0; j < center_width/2; j++) {
			int row_start = center_start + i*center_width;
			std::swap(centers[row_start+j], centers[row_start+center_width-j-1]);	
		}
	}
}

void Cube::rotate_face(const Face face) {
	//corners are numbered as follows
	// 0 |  | 1
	//---------
	//   |  | 
	//---------
	// 3 |  | 2
	int face_corners[6][4] = {
		{4,7,3,0},
		{4,5,6,7},
		{5,4,0,1},
		{3,2,1,0},
		{7,6,2,3},
		{6,5,1,2},
	};

	//edges are numbered as follows
	//   | 0 | 
	//----------
	// 3 |   | 1
	//----------
	//   | 2 | 
	int face_edges[6][4] = {
		{11,7,3,4},
		{8,9,10,11},
		{8,4,0,5},
		{2,1,0,3},
		{10,6,2,7},
		{9,5,1,6},
	};

	//rotate the centers
	reverse_center_rows(face);
	transpose_center(face);

	//rotate the corners
	auto& corner_shifts = face_corners[face_numbers[face]];
	for (int i = 0; i < 4; i++) {
		if ((corner_shifts[(i+1)%4] < 4 && corner_shifts[i] < 4) || 
				(corner_shifts[(i+1)%4] > 4 && corner_shifts[i] > 4)) {
			rotate_corner(corner_shifts[i]);
		
		}
		else {
			rotate_corner(corner_shifts[i]);
			rotate_corner(corner_shifts[i]);
		}
	}

	unsigned char temp = corners[corner_shifts[3]];
	for (int i = 2; i >= 0; i--) {
		corners[corner_shifts[i+1]] = corners[corner_shifts[i]];
	}
	corners[corner_shifts[0]] = temp;


	//rotate the edges
	auto& edge_shifts = face_edges[face_numbers[face]];
	int edge_length = size-2;
	for (int i = 0; i < edge_length; i++) {
		for (int j = 0; j < 4; j++) {
			flip_edge(edge_shifts[j]*edge_length + i);	
		}
		unsigned char temp = edges[edge_shifts[3]*edge_length + i];
		for (int j = 2; j >= 0; j--) {
			edges[edge_shifts[j+1]*edge_length + i] = edges[edge_shifts[j]*edge_length + i];
		}
		edges[edge_shifts[0]*edge_length + i] = temp;
	}
}

void Cube::rotate_slice(const Face face, const int layer) {
	std::unordered_set<Face> far_faces = {Face::BACK, Face::RIGHT, Face::TOP};

	//edges are numbered as follows
	// 0 |  | 1
	//---------
	//   |  | 
	//---------
	// 3 |  | 2
	int slice_edges[6][4] = {
		{8,10,2,0},
		{4,5,6,7},
		{9,11,3,1},
		{7,6,5,4},
		{11,9,1,3},
		{10,8,0,2},
	};

	//centers are specified as follows
	//   | 0 | 
	//----------
	// 3 |   | 1
	//----------
	//   | 2 | 
	int slice_centers[6][4] = {
		{1,4,3,2},
		{2,5,4,0},
		{1,0,3,5},
		{4,5,2,0},
		{1,5,3,0},
		{1,2,3,4},
	};

	//rotate the edges
	auto& edge_shifts = slice_edges[face_numbers[face]];
	int inner_layer;

	if (far_faces.count(face) > 0) {
		inner_layer = size-layer-2;	
	}
	else {
		inner_layer = layer-1;	
	}
	
	for (int i = 0; i < 4; i++) {
		flip_edge(edge_shifts[i]+inner_layer);
	}
	unsigned char temp = edges[edge_shifts[3]+inner_layer];
	for (int i = 2; i >= 0; i--) {
		edges[edge_shifts[i+1]+inner_layer] = edges[edge_shifts[i]+inner_layer];
	}
	edges[edge_shifts[0]+inner_layer] = temp;

	//rotate the centers
	auto& center_shifts = slice_centers[face_numbers[face]];
	int center_width = size-2;
	for (int i = 0; i < center_width; i++) {
		int center_index;
		if (face == Face::TOP || face == Face::BOTTOM) {
			center_index = i + inner_layer;	
		}
		else {
			center_index = (i*center_width)+inner_layer;	
		}

		unsigned char temp = centers[center_shifts[3]*std::pow(center_width,2)+center_index];
		for (int j = 2; j >= 0; j--) {
			centers[center_shifts[j+1]*std::pow(center_width,2)+center_index] = 
				centers[center_shifts[j]*std::pow(center_width,2)+center_index];
		}
		centers[center_shifts[0]*std::pow(center_width,2)+center_index] = temp;
	}
}

void Cube::rotate(const Twist& twist) {
	for (int i = twist.layer; i >= (twist.wide_turn ? 0 : twist.layer); i--) {
		if (i == 0) {
			if (twist.degrees == 90) {
				rotate_face(twist.face);	
			}
			else {
				rotate_face(twist.face);	
				rotate_face(twist.face);	
				rotate_face(twist.face);	
			}
		}
		else if (i == size - 1) {
			rotate(Twist(-twist.degrees, opposing_faces[twist.face]));
		}
		else {
			if (twist.degrees == 90) {
				rotate_slice(twist.face, twist.layer);	
			}
			else {
				rotate_slice(twist.face, twist.layer);	
				rotate_slice(twist.face, twist.layer);	
				rotate_slice(twist.face, twist.layer);	
			}
		}	
	}	
}
