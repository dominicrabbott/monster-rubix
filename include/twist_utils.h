#ifndef COMMON_TWISTS
#define COMMON_TWISTS 

#include <vector>
#include <array>
#include "twist_sequence.h"
#include "face.h"

namespace cube {
	class CubeBase;
};

namespace ai {
	namespace TwistUtils {
		const std::array<int, 2> DEGREES = {90, -90};
		const std::array<cube::Face, 3> AXIS_FACES = {cube::Face::LEFT, cube::Face::BOTTOM, cube::Face::BACK};

		//generates a set of TwistSequences that represent 90 and -90
		//degree rotations of every face
		std::vector<TwistSequence> generate_face_twists();

		//generates a set of TwistSequences that represent 90 and -90
		//degree rotations of the whole cube around every axis
		std::vector<TwistSequence> generate_cube_rotations(const cube::CubeBase& cube);
	};
}
#endif
