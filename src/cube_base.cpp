#include "cube_base.h"
#include "twist.h"
#include <cmath>
#include <iostream>
#include <unordered_set>
#include <cassert>
#include <boost/functional/hash.hpp>
#include <algorithm>

using namespace cube;

CubeBase::CubeBase(const int size) : 
	size(size),
	edge_width(size-2) {}

CubeBase::CubeBase(const CubeBase& cube) : 
	size(cube.size),
	edge_width(cube.edge_width) {}

void CubeBase::shift_pieces(uint8_t* pieces, const std::array<int, 4> indecies, const int degrees) {
	int shifts = degrees == 90 ? 1 : 3;
	int temp = pieces[indecies[0]];
	for (int i = 4-shifts; i != 0; i = (i-shifts+4)%4) {
		pieces[indecies[(i+shifts)%4]] = pieces[indecies[i]];	
	}
	pieces[indecies[shifts]] = temp;
}

std::unique_ptr<uint8_t[]> CubeBase::copy_pieces(const uint8_t* array, const int size) {
	std::unique_ptr<uint8_t[]> result = std::make_unique<uint8_t[]>(size);
	std::copy(array, array+size, result.get());

	return result;
}
