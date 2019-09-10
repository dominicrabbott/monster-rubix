#include <unordered_map>

#include "cube_manipulator.h"
#include "face.h"
#include "piece.h"

using namespace cube;

template <typename T>
void CubeManipulator<T>::transpose(const std::vector<std::vector<T*>>& matrix) {
	using std::swap;
	for (int i = 0; i < matrix.size()-1; i++) {
		for (int j = i+1; j < matrix.size(); j++) {
			swap(*matrix[i][j], *matrix[j][i]);	
		}	
	}
}

template <typename T>
void CubeManipulator<T>::reverse_rows(const std::vector<std::vector<T*>>& matrix) {
	using std::swap;
       	for (const auto& row : matrix) {
	       for (int col = 0; col < row.size()/2; col++) {
	      		swap(*row[col], *row[row.size()-1-col]); 
	       }
       	}
}

template <typename T>
void CubeManipulator<T>::rotate(const std::vector<std::vector<T*>>& matrix, const int degrees) {
	if (degrees == 90) {
		transpose(matrix);
		reverse_rows(matrix);	
	}

	else if (degrees == -90) {
		reverse_rows(matrix);
		transpose(matrix);	
	}
}

template <typename T>
std::vector<std::vector<T*>> CubeManipulator<T>::find_layer(std::vector<std::vector<std::vector<T>>>& cube, const Face face, const int layer) {
	std::vector<std::vector<T*>> result;


	for (int i = 0; i < cube.size(); i++) {
		result.push_back(std::vector<T*>());

		for (int j = 0; j < cube.size(); j++) {
			int coords[] = {0,0,0};

			switch (face) {
				case Face::FRONT:
					coords[0] = i;
					coords[1] = j;
					coords[2] = cube.size() - 1 - layer;
					break;
				case Face::BACK:
					coords[0] = i;
					coords[1] = j;
					coords[2] = layer;
					break;
				case Face::TOP:
					coords[0] = i;
					coords[1] = cube.size() - 1 - layer;
					coords[2] = j;
					break;
				case Face::BOTTOM:
					coords[0] = i;
					coords[1] = layer;
					coords[2] = j;
					break;
				case Face::RIGHT:
					coords[0] = cube.size() - 1 - layer;
					coords[1] = i;
					coords[2] = j;
					break;
				case Face::LEFT:
					coords[0] = layer;
					coords[1] = i;
					coords[2] =j;
					break;

			};

			result[i].push_back(&cube[coords[0]][coords[1]][coords[2]]);	
		}	
	}

	return result;
}

template class CubeManipulator<Piece>;
