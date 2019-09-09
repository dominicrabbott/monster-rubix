#include <algorithm>
#include <vector>

#include "piece.h"
#include "color.h"
#include "face.h"

using namespace cube;

std::unordered_map<Face, std::vector<Face>> Piece::adjacent_faces = {
	{Face::FRONT, {Face::TOP, Face::RIGHT, Face::BOTTOM, Face::LEFT}},
	{Face::BACK,  {Face::TOP, Face::LEFT, Face::BOTTOM, Face::RIGHT}},
	{Face::RIGHT, {Face::TOP, Face::BACK, Face::BOTTOM, Face::FRONT}},
	{Face::LEFT, {Face::TOP, Face::FRONT, Face::BOTTOM, Face::BACK}},
	{Face::TOP,  {Face::BACK, Face::RIGHT, Face::FRONT, Face::LEFT}},
	{Face::BOTTOM, {Face::FRONT, Face::RIGHT, Face::BACK, Face::LEFT}},
};

Color Piece::get_face(const Face face) const {
	auto iter = faces.find(face);

	if (iter == faces.end()) {
		return Color::BLACK;	
	}

	return iter -> second;
}

void Piece::rotate(const int degrees, const Face face) {
	std::unordered_map<Face, Color> faces_tmp = faces;
	std::vector<Face>& rotated_faces = adjacent_faces[face];
	int face_order[] = {0,1,2,3};

	if (degrees == -90) {
		std::reverse(face_order, face_order+3);
	}	

	for (int i = 0; i < 4; i++) {
		Face from = rotated_faces[face_order[i]];
		Face to = rotated_faces[face_order[(i+1)%4]];

		if (faces_tmp.find(from) == faces_tmp.end()) {
			faces.erase(to);	
		}

		else {
			faces[to] = faces_tmp[from];
		}
	}
}
