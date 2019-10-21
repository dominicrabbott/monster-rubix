#ifndef CUBE_STATE_H
#define CUBE_STATE_H

#include <vector>
#include <memory>
#include <boost/optional.hpp>

#include "cube.h"
#include "twist_sequence.h"

namespace ai {
	//represents a node in a state-space for a cube
	struct CubeState {
		//pointer this state's parent state
		std::shared_ptr<CubeState> parent;
		//state of the cube at this point
		cube::Cube cube;
		//sequence of twists made to get from the parent to this state
		boost::optional<TwistSequence> twist_seq;

		CubeState(const std::shared_ptr<CubeState> parent, const cube::Cube& cube, const TwistSequence& twist_seq) : 
			parent(parent), cube(cube), twist_seq(twist_seq) {}
		
		CubeState(const std::shared_ptr<CubeState> parent, const cube::Cube&& cube, const TwistSequence& twist_seq) : 
			parent(parent), cube(cube), twist_seq(twist_seq) {}

		CubeState(const cube::Cube& cube) : 
			cube(cube) {}
	};
}

#endif
