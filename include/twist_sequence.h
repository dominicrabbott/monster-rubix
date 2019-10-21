#ifndef MOVE_SEQUENCE_H
#define MOVE_SEQUENCE_H

#include <vector>

#include "twist.h"

namespace ai {
	//Represents a series of twists that are viewed as one, atomic sequence. This struct is
	//used to represent rubix cube 'algorithms', which are a series of moves used to accomplish
	//a step in solving the cube (not to be confused with computer algorithms).
	struct TwistSequence {
		std::vector<cube::Twist> twists;

		TwistSequence(const cube::Twist twist) : twists({twist}) {}
		TwistSequence(const std::vector<cube::Twist> twists) : twists(twists) {}
	};
}

#endif
