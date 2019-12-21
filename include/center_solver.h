#ifndef CENTER_SOLVER_H
#define CENTER_SOLVER_H

#include "cube_centers.h"
#include "twist_listener.h"
#include "twist.h"
#include "heuristic_cube_state.h"
#include "hash.h"
#include <array>
#include <unordered_set>
#include <boost/functional/hash.hpp>
#include <memory>
#include <boost/optional.hpp>

namespace ai {
	class CenterSolver {
		private:
			//callable that returns a value determined by applying a heuristic 
			//to the given cube object
			struct CenterHeuristic {
				int operator()(const cube::CubeCenters& centers);
			};

			typedef HeuristicCubeState<cube::CubeCenters, CenterHeuristic> State;

			//the observers that are notified when a twist is made to solve the cube
			std::vector<TwistListener*> twist_listeners;

			//returns the twists made to arrive at the given state from the root state
			std::vector<cube::Twist> trace_twists(const State* state);

			//performs the given twists on the CubeCenters object and updates the TwistListeners
			//of the twists made
			void rotate(const std::vector<cube::Twist>& twists, cube::CubeCenters& centers);

			//generates a set of commutators that, when combined with rotations of the whole 
			//cube and rotations of each face, can be used to swap any two centers in the given
			//CubeCenters object
			std::vector<TwistSequence> generate_commutators(const cube::CubeCenters& centers);
			
			//returns the TwistSequences that are used to build the state-space during the first strategy
			//used to solve the cube. The first strategy consists of 90 and -90 degree rotations to every
			//layer of the cube. 
			std::vector<TwistSequence> generate_strategy_1(const cube::CubeCenters& centers);
			
			//returns the TwistSequences that are used to build the state-space during the second strategy
			//used to solve the cube. The second strategy consists of 90 and -90 degree rotations of every face,
			//90 and -90 degree rotations of the whole cube around every axis, and the set of center commutators
			//needed to swap any 2 centers in the given CubeCenters object
			std::vector<TwistSequence> generate_strategy_2(const cube::CubeCenters& centers);
		public:
			//adds an listener that is to be notified when twists are made to solve the cube
			void add_twist_listener(TwistListener* listener) {twist_listeners.push_back(listener);}

			//solves the given cube object
			void solve(cube::CubeCenters& centers);
	
	};
}


#endif 
