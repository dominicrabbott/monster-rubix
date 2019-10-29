#include <cmath>
#include <iostream>
#include <unordered_set>
#include <cassert>
#include <boost/functional/hash.hpp>

#include "cube_base.h"
#include "twist.h"

using namespace cube;

std::unordered_map<Face, Face> CubeBase::opposing_faces = {
	{Face::FRONT, Face::BACK},
	{Face::BACK, Face::FRONT},
	{Face::RIGHT, Face::LEFT},
	{Face::LEFT, Face::RIGHT},
	{Face::TOP, Face::BOTTOM},
	{Face::BOTTOM, Face::TOP},
};

CubeBase::CubeBase(const int size) : 
	size(size),
	edge_width(size-2) {}

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
