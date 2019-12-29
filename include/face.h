#ifndef FACE_H
#define FACE_H

#include <array>
#include <unordered_map>
#include <iostream>
	
namespace cube {

	enum class Face {LEFT = 0, TOP = 1, BACK = 2, BOTTOM = 3, FRONT = 4, RIGHT = 5};

	const std::array<Face,6> ALL_FACES = {
		Face::RIGHT,	
		Face::LEFT,	
		Face::TOP,	
		Face::BOTTOM,	
		Face::FRONT,	
		Face::BACK,
	};

	const std::unordered_map<Face, Face> OPPOSING_FACES = {
		{Face::FRONT, Face::BACK},
		{Face::BACK, Face::FRONT},
		{Face::RIGHT, Face::LEFT},
		{Face::LEFT, Face::RIGHT},
		{Face::TOP, Face::BOTTOM},
		{Face::BOTTOM, Face::TOP},
	};
	
	std::ostream& operator<<(std::ostream& stream, const Face& face);
}

#endif
