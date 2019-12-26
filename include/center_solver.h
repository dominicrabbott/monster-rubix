#ifndef CENTER_SOLVER_H
#define CENTER_SOLVER_H

#include "cube_centers.h"
#include "twist_listener.h"
#include "twist_provider.h"
#include "twist.h"
#include "heuristic_cube_state.h"
#include "hash.h"
#include <array>
#include <unordered_set>
#include <boost/functional/hash.hpp>
#include <memory>
#include <cmath>
#include <boost/optional.hpp>

namespace ai {
	class CenterSolver : public TwistProvider {
		private:
			//callable that returns a value determined by applying a heuristic 
			//to the given cube object
			struct CenterHeuristic {
				int operator()(const cube::CubeCenters& centers);
			};

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

			//counts the number of center pieces solved in the given CubeCenters object
			int count_solved_pieces(const cube::CubeCenters& centers);
		public:
			
			//solves the given cube object
			void solve(const cube::CubeCenters& root_state);
	
	};
}


#endif 
