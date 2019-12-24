#ifndef SEARCH_H
#define SEARCH_H

#include <vector>
#include <functional>
#include "twist.h"
#include "twist_sequence.h"
#include "heuristic_cube_state.h"

namespace ai {
	template<typename StateType, typename Heuristic>
	class Search {
		private:
			typedef HeuristicCubeState<StateType, Heuristic> State;
			std::vector<cube::Twist> trace_twists(const State* state);

		public:
			//performs a best-first search using the given set of twist_sequences to build the state-space and using the 
			//'Heuristic' template paramater to guide the search. Returns a vector of Twists that led to the state that made
			//'is_finished' return true
			std::vector<cube::Twist> best_first_search(const StateType& root_state, const std::vector<TwistSequence> twist_sequences, const std::function<bool(const StateType&)>& is_finished);
	};
};

#include "search.hpp"

#endif
