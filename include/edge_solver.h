#ifndef EDGE_SOLVER
#define EDGE_SOLVER

#include <vector>
#include <unordered_map>
#include <array>
#include "twist_listener.h"
#include "twist_sequence.h"
#include "twist.h"
#include "heuristic_cube_state.h"
#include "cube.h"
#include "twist_provider.h"

namespace ai {
	class EdgeSolver : public TwistProvider {
		private:
			//Heuristic used for the best-first search to solve the first 10 edges.
			//The heuristic returns the number of edge pieces left to solve
			struct EdgeHeuristic {
				struct EdgeAssignment {
					int edge;
					int piece;
					int placed_pieces;
					EdgeAssignment(const int edge, const int piece, const int placed_pieces) :
						edge(edge), piece(piece), placed_pieces(placed_pieces) {}
				
				};
				int operator()(const cube::Cube& cube);	
			};

			//Heuristic used for the best-first search to solve the last 2 edges.
			//The heuristic returns the number of unsolved edges not in edges 8 and 10, which
			//is where the last unsolved edges need to be to be solved
			struct LastTwoEdgesHeuristic {
				int operator()(const cube::Cube& cube);
			};

			std::array<int, 2> degrees = {-90, 90};
			int edge_count = 12;

			//generates the commutators nessesary to swap any two edges on the front-top
			//and front-back edges of the cube
			std::vector<TwistSequence> generate_edge_commutators(const cube::Cube& cube);

			//This function generates an algorithm that is nessesary
			//to solve the last two edges of odd cubes, which flips the center 
			//edge of the top-front face. This function should only be called with 
			//odd cubes
			TwistSequence generate_edge_flipper(const cube::Cube& cube);

			//generates 90 and -90 degree twists for every face
			std::vector<TwistSequence> generate_all_face_twists();

			//returns true if the specified edge is solved on the given cube
			bool edge_is_solved(const cube::Cube& cube, const int edge);

			//solves 10 edges on the cube, leaving 2 unsolved
			std::vector<cube::Twist> solve_first_ten_edges(const cube::Cube& cube);

			//solves the last 2 edges on the cube, finishing the solution of the edges
			std::vector<cube::Twist> solve_last_two_edges(const cube::Cube& cube);
			
			friend struct LastTwoEdgesHeuristic;
		public:
			//solves the edges on the cube
			void solve(const cube::Cube& cube);
	};
}

#endif
