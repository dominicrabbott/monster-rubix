#include <unordered_map>
#include <iostream>

#include "cube.h"
#include "twist.h"
#include "face.h"
#include "color.h"
#include "piece.h"
#include "cube_manipulator.h"

using namespace cube;

Cube::Cube(const int size) {
	for (int x = 0; x < size; x++) {
		cube.push_back(std::vector<std::vector<Piece>>());
		for (int y = 0; y < size; y++) {
			cube[x].push_back(std::vector<Piece>());
			for (int z = 0; z < size; z++) {
				cube[x][y].push_back(Piece());	
			}	
		}	
	}

	std::unordered_map<Face, Color> colors;
	colors.insert(std::make_pair(Face::FRONT, Color::GREEN));
	colors.insert(std::make_pair(Face::BACK, Color::BLUE));
	colors.insert(std::make_pair(Face::RIGHT, Color::ORANGE));
	colors.insert(std::make_pair(Face::LEFT, Color::RED));
	colors.insert(std::make_pair(Face::TOP, Color::YELLOW));
	colors.insert(std::make_pair(Face::BOTTOM, Color::WHITE));
	for (const Face face : (Face[]) {Face::FRONT, Face::BACK, Face::TOP, Face::BOTTOM, Face::RIGHT, Face::LEFT}) {
		std::vector<std::vector<Piece*>> layer = CubeManipulator<Piece>::find_layer(cube, face, 0);
		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				layer[i][j] -> set_face(face, colors[face]);	
			}	
		}

	}

}

void Cube::rotate(const Twist& move) {
	std::vector<int> layers_rotated = {move.layer};

	if (move.wide_turn) {
		for (int i = 0; i < move.layer; i++) {
			layers_rotated.push_back(i);
		}
	}

	for (const int layer : layers_rotated) {
		std::vector<std::vector<Piece*>> layer_pieces = CubeManipulator<Piece>::find_layer(cube, move.face, layer);
		for (auto& row : layer_pieces) {
			for (auto piece_ptr : row) {
				piece_ptr -> rotate(move.degrees, move.face);	
			}			
		}

		CubeManipulator<Piece>::rotate(layer_pieces, move.degrees);
	}
}

std::vector<std::vector<Color>> Cube::get_face(const Face face) {
	std::vector<std::vector<Color>> face_colors;

	for (const auto& row : CubeManipulator<Piece>::find_layer(cube, face, 0)) {
		face_colors.push_back(std::vector<Color>());
		for (const auto piece_ptr : row) {
			face_colors.back().push_back(piece_ptr -> get_face(face));
		}	
	}

	return face_colors;
}
