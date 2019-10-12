#include <cmath>
#include <iostream>
#include <unordered_set>
#include <cassert>
#include <boost/functional/hash.hpp>

#include "cube_base.h"
#include "twist.h"

using namespace cube;

std::unordered_map<Face, int> CubeBase::face_numbers = {
	{Face::LEFT, 0},
	{Face::TOP, 1},
	{Face::BACK, 2},
	{Face::BOTTOM, 3},
	{Face::FRONT, 4},
	{Face::RIGHT, 5},

};

CubeBase::CubeBase(const int size) : 
	size(size),
	edge_width(size-2) {
		assert(size <= 8 && "Cube class can only represent 8x8x8 cubes or smaller");
	}

CubeBase::CubeBase(const CubeBase& cube) : 
	size(cube.size),
	edge_width(cube.edge_width) {}

void CubeBase::shift_pieces(unsigned char* pieces, int* indecies, int degrees, int offset) {
	int shifts = degrees == 90 ? 1 : 3;
	
	int temp = pieces[indecies[0]+offset];
	for (int i = 4-shifts; i != 0; i = (i-shifts+4)%4) {
		pieces[indecies[(i+shifts)%4]+offset] = pieces[indecies[i]+offset];	
	}
	pieces[indecies[shifts]+offset] = temp;
}
