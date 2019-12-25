#ifndef MOVE_SEQUENCE_H
#define MOVE_SEQUENCE_H

#include <vector>

#include "twist.h"

namespace ai {
	//Represents a series of twists that are viewed as one, atomic sequence. This is
	//used to represent rubix cube 'algorithms', which are a series of moves used to accomplish
	//a step in solving the cube (not to be confused with computer algorithms).
	typedef std::vector<cube::Twist> TwistSequence;
}

#endif
