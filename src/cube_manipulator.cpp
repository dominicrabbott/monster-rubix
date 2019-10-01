#include <unordered_map>
#include <cassert>

#include "cube_manipulator.h"
#include "face.h"

using namespace ui;
using namespace Ogre;

void CubeManipulator::transpose(const std::vector<std::vector<SceneNode**>>& matrix) {
	using std::swap;
	for (int i = 0; i < matrix.size()-1; i++) {
		for (int j = i+1; j < matrix.size(); j++) {
			swap(*matrix[i][j], *matrix[j][i]);	
		}	
	}
}

void CubeManipulator::reverse_rows(const std::vector<std::vector<SceneNode**>>& matrix) {
	using std::swap;
       	for (const auto& row : matrix) {
	       for (int col = 0; col < row.size()/2; col++) {
	      		swap(*row[col], *row[row.size()-1-col]); 
	       }
       	}
}

void CubeManipulator::rotate(const std::vector<std::vector<SceneNode**>>& matrix, const int degrees) {
	assert(degrees == 90 || degrees == -90 && "Only rotations of 90 and -90 degrees are allowed");

	if (degrees == 90) {
		transpose(matrix);
		reverse_rows(matrix);	
	}

	else if (degrees == -90) {
		reverse_rows(matrix);
		transpose(matrix);	
	}
}

std::vector<std::vector<SceneNode**>> CubeManipulator::find_layer(std::vector<std::vector<std::vector<SceneNode*>>>& cube, const cube::Face face, const int layer) {
	std::vector<std::vector<SceneNode**>> result;


	for (int i = 0; i < cube.size(); i++) {
		result.push_back(std::vector<SceneNode**>());

		for (int j = 0; j < cube.size(); j++) {
			std::array<int, 3> coords = layer_coords(i,j,face,layer,cube.size());
			
			result[i].push_back(&cube[coords[0]][coords[1]][coords[2]]);	
		}	
	}

	return result;
}

std::vector<std::vector<SceneNode* const*>> CubeManipulator::const_find_layer(const std::vector<std::vector<std::vector<SceneNode*>>>& cube, const cube::Face face, const int layer) {
	std::vector<std::vector<SceneNode* const*>> result;

	for (int i = 0; i < cube.size(); i++) {
		result.push_back(std::vector<SceneNode* const*>());

		for (int j = 0; j < cube.size(); j++) {
			std::array<int, 3> coords = layer_coords(i,j,face,layer,cube.size());
			
			result[i].push_back(&cube[coords[0]][coords[1]][coords[2]]);	
		}	
	}

	return result;

}

std::array<int, 3> CubeManipulator::layer_coords(const int i, const int j, const cube::Face face, const int layer, const int cube_size) {
	std::array<int, 3> coords = {0,0,0};

	switch (face) {
		case cube::Face::FRONT:
			coords[0] = i;
			coords[1] = j;
			coords[2] = cube_size - 1 - layer;
			break;
		case cube::Face::BACK:
			coords[0] = i;
			coords[1] = j;
			coords[2] = layer;
			break;
		case cube::Face::TOP:
			coords[0] = j;
			coords[1] = cube_size - 1 - layer;
			coords[2] = i;
			break;
		case cube::Face::BOTTOM:
			coords[0] = j;
			coords[1] = layer;
			coords[2] = i;
			break;
		case cube::Face::RIGHT:
			coords[0] = cube_size - 1 - layer;
			coords[1] = i;
			coords[2] = j;
			break;
		case cube::Face::LEFT:
			coords[0] = layer;
			coords[1] = i;
			coords[2] = j;
			break;

	};

	return coords;
} 
