#ifndef HEURISTIC_CUBE_STATE_H
#define HEURISTIC_CUBE_STATE_H

#include <memory>
#include <boost/optional.hpp>
#include "twist_sequence.h"

namespace ai {
	template<typename StateType, typename Heuristic>
	struct HeuristicCubeState {
		std::shared_ptr<HeuristicCubeState> parent;
		StateType cube;
		boost::optional<TwistSequence> twist_seq;
		int score;
		HeuristicCubeState(const std::shared_ptr<HeuristicCubeState> parent, const StateType& cube, const TwistSequence& twist_seq) : parent(parent), cube(cube), twist_seq(twist_seq), score(Heuristic()(cube)){}
		HeuristicCubeState(const std::shared_ptr<HeuristicCubeState> parent, const StateType&& cube, const TwistSequence& twist_seq) : parent(parent), cube(cube), twist_seq(twist_seq), score(Heuristic()(cube)) {}
		HeuristicCubeState(const StateType& cube) : cube(cube), score(Heuristic()(cube)) {}
	};
}

#endif
