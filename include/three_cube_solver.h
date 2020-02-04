#ifndef THREE_CUBE_SOLVER
#define THREE_CUBE_SOLVER

#include <boost/serialization/vector.hpp>
#include <algorithm>
#include <string>
#include <boost/serialization/unordered_map.hpp>
#include <unordered_set>
#include <functional>
#include <memory>
#include <array>
#include "twist.h"
#include "twist_sequence.h"
#include "cube_state.h"
#include "twist_provider.h"
#include "cube.h"

namespace cube {
	class CombinedCube;
	class CubeCenters;
}

namespace ai {
	class ThreeCubeSolver : public TwistProvider {
		private:
			typedef std::unordered_map<std::vector<bool>, std::vector<cube::Twist>> LookupTable;
			typedef std::function<std::vector<bool>(const cube::Cube&)> Encoder;
			typedef CubeState<cube::Cube> State;
			static constexpr int stage_count = 4;

			//lookup tables for each of the stages
			std::array<LookupTable, stage_count> tables;

			//Encoder callables for each of the stages
			std::array<Encoder, stage_count> encoders = {
				std::bind(&ThreeCubeSolver::encode_g1, this, std::placeholders::_1),
				std::bind(&ThreeCubeSolver::encode_g2, this, std::placeholders::_1),
				std::bind(&ThreeCubeSolver::encode_g3, this, std::placeholders::_1),
				std::bind(&ThreeCubeSolver::encode_g4, this, std::placeholders::_1),
			};

			//maps the edges to the slices they belong in
			std::unordered_map<int, int> edge_slices {
				{4,0}, {5,0}, {6,0}, {7,0},
				{0,1}, {2,1}, {8,1}, {10,1},
				{3,2}, {1,2}, {11,2}, {9,2},
			};

			//directory the lookup tables are stored to
			std::string table_dir = "tables/";

			//returns the edge position of the specified edge on a reduced
			//cube
			int get_edge_pos(const cube::Cube& cube, const int edge) {
				return cube.get_edge_pos(edge*cube.get_edge_width());	
			}
			
			//returns the edge orientation of the specified edge on a reduced
			//cube
			int get_edge_orientation(const cube::Cube& cube, const int edge) {
				return cube.get_edge_orientation(edge*cube.get_edge_width());	
			}

			//orients the cube so it is in its 'natural' orientation, ie each center is
			//in the position it was before the cube was scrambled
			std::vector<cube::Twist> orient_cube(const cube::CubeCenters& centers);

			//returns true if the parity of the corner permutation is even
			bool even_corner_parity(const cube::Cube& cube);

			//returns true if the parity of the edge permutation is even
			bool even_edge_parity(const cube::Cube& cube);

			//returns true if the parity of the given sequence is even
			bool even_parity(const std::vector<int>& sequence);

			//returns the twists needed to solve orientation and permutation parity issues 
			//in even-dimensioned cubes
			TwistSequence solve_parity(const cube::Cube& cube);
			
			//returns a vector holding the twists needed to transition from the given state
			//to the root state, which is the goal state of a stage
			std::vector<cube::Twist> get_twists(const State* state);

			//returns a vector that contains the twist sequences that can be made at a stage. If a face
			//exists in the restricted_faces parameter, only 180 degree turns of that face are allowed. Otherwise, 90 and -90 degree
			//turns are allowed
			std::vector<TwistSequence> generate_twist_sequences(const std::unordered_set<cube::Face>& restricted_faces);

			//attemps to load a lookup table located at the given path. Returns true and populates the given lookup table
			//with the loaded data if the load was successful. If the load was not successful because the file doesn't exist yet,
			//this function returns false.
			bool load_lookup_table(LookupTable& table, std::string file_path);

			//serializes the given lookup table to the given path 
			void save_lookup_table(const LookupTable& table, std::string file_path);
			
			//helper function for creating encodings. Pushes the last n bits of 'data' to the end
			//of 'encoding', where n is specified by the 'bits' paramater.
			void build_encoding(std::vector<bool>& encoding, const uint8_t data, const int bits);

			//Encoder functions for each stage. Each encoder function takes a cube object and
			//encodes it into a vector of uint8_t. The encoding represents only what 
			//is relevant to its corresponding stage, and nothing more.
			std::vector<bool> encode_g1(const cube::Cube& cube);
			std::vector<bool> encode_g2(const cube::Cube& cube);
			//A modified G3 encoding us used that is easier to code. Credit to Stefan Pochmann:
			//http://www.stefan-pochmann.info/spocc/other_stuff/tools/solver_thistlethwaite/solver_thistlethwaite.txt 	
			std::vector<bool> encode_g3(const cube::Cube& cube);
			std::vector<bool> encode_g4(const cube::Cube& cube);
			
			//creates a lookup table using the given encoder and using the given twist sequences
			//to create the state-space	
			LookupTable create_lookup_table(const Encoder encoder, const std::vector<TwistSequence> twist_sequences);

			//notifies the twist listeners of the twists in the given twist sequence and performs those
			//twists on the given cube object
			void execute_partial_solution(const TwistSequence& twist_sequence, cube::CombinedCube& comb_cube);

		public:
			//constructor loads the lookup tables
			ThreeCubeSolver();

			//solves the given cube object and returns a vector that contains the twists that were made
			//to solve the cube
			void solve(const cube::CombinedCube& comb_cube);
	};
}

#endif
