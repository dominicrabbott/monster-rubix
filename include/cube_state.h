#ifndef CUBE_STATE_H
#define CUBE_STATE_H

#include <vector>
#include <memory>
#include <boost/optional.hpp>
#include "twist_sequence.h"

namespace ai {
	//represents a node in a state-space of a cube
	template <typename StateType>
	struct CubeState {
		//pointer this state's parent state
		std::shared_ptr<CubeState> parent;
		//state of the cube at this point
		StateType cube;
		//sequence of twists made to get from the parent to this state
		boost::optional<TwistSequence> twist_seq;

		CubeState(const std::shared_ptr<CubeState> parent, const StateType& cube, const TwistSequence& twist_seq) : 
			parent(parent), cube(cube), twist_seq(twist_seq) {}
		
		CubeState(const std::shared_ptr<CubeState> parent, const StateType&& cube, const TwistSequence& twist_seq) : 
			parent(parent), cube(cube), twist_seq(twist_seq) {}

		CubeState(const StateType& cube) : 
			cube(cube) {}
	};
}

#endif
