#include "twist_utils.h"
#include "cube_base.h"
#include "twist.h"

using namespace ai;
using namespace TwistUtils;

std::vector<TwistSequence> TwistUtils::generate_face_twists() {
	std::vector<TwistSequence> twist_sequences;
	for (const auto face : cube::ALL_FACES) {
		for (const int deg : DEGREES) {
			twist_sequences.push_back(TwistSequence({cube::Twist(deg, face)}));
		}
	}

	return twist_sequences;
}

std::vector<TwistSequence> TwistUtils::generate_cube_rotations(const cube::CubeBase& cube) {
	using namespace cube;
	std::vector<TwistSequence> twist_sequences;
	for (const auto face : AXIS_FACES) {
		for (const int deg : DEGREES) {
			twist_sequences.push_back(TwistSequence({Twist({deg, face, cube.get_size()-1})}));
		}	
	}
	
	return twist_sequences;
}
