#ifndef SEARCH_H
#define SEARCH_H

#include <vector>
#include <functional>
#include "twist.h"
#include "twist_sequence.h"

namespace ai {
	namespace search {
		template<typename StateType>
		std::vector<cube::Twist> trace_twists(const StateType* state);

		//performs a best-first search using the given set of TwistSequences to build the state-space and 
		//using the 'Heuristic' template paramater to guide the search. Returns a vector of Twist 
		//objects that led to the state that made 'is_finished' return true
		template<typename CubeType, typename Heuristic>
		std::vector<cube::Twist> best_first_search(
			const CubeType& root_state, 
			const std::vector<TwistSequence> twist_sequences, 
			const std::function<bool(const CubeType&)>& is_finished);
		
		//performs a breadth-first search using the given set of TwistSequences to build the state-space.
		//Returns a vector of Twist objects that led to the state that made 'is_finished' return true
		template<typename CubeType>
		std::vector<cube::Twist> breadth_first_search(
			const CubeType& root_state, 
			const std::vector<TwistSequence> twist_sequences,
			const std::function<bool(const CubeType&)>& is_finished);
	};
};

#include "search.hpp"

#endif
