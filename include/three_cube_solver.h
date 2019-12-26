#ifndef THREE_CUBE_SOLVER
#define THREE_CUBE_SOLVER

#include <boost/serialization/vector.hpp>
#include <algorithm>
#include <string>
#include <boost/serialization/unordered_map.hpp>
#include <unordered_set>
#include <functional>
#include <memory>

#include "twist.h"
#include "twist_sequence.h"
#include "cube_state.h"
#include "hash.h"

namespace cube {
	class Cube;
}

namespace ai {
	class ThreeCubeSolver {
		private:
			typedef std::unordered_map<std::vector<uint8_t>, std::vector<cube::Twist>> LookupTable;
			typedef std::function<std::vector<uint8_t>(const cube::Cube&)> Encoder;
			typedef CubeState<cube::Cube> State;

			//lookup tables for each of the stages
			LookupTable tables[4];

			//Encoder callables for each of the stages
			Encoder encoders[4] = {
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

			//returns true if the parity of the corners and edges of the cube is even
			bool even_parity(const cube::Cube& cube);

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

			//Encoder functions for each stage. Each encoder function takes a cube object and
			//encodes it into a vector of uint8_t. The encoding represents only what 
			//is relevant to its corresponding stage, and nothing more.
			std::vector<uint8_t> encode_g1(const cube::Cube& cube);
			std::vector<uint8_t> encode_g2(const cube::Cube& cube);
			//A modified G3 encoding us used that is easier to code. Credit to Stefan Pochmann:
			//http://www.stefan-pochmann.info/spocc/other_stuff/tools/solver_thistlethwaite/solver_thistlethwaite.txt 	
			std::vector<uint8_t> encode_g3(const cube::Cube& cube);
			std::vector<uint8_t> encode_g4(const cube::Cube& cube);
			
			//creates a lookup table using the given encoder and using the given twist sequences
			//to create the state-space	
			LookupTable create_lookup_table(const Encoder encoder, const std::vector<TwistSequence> twist_sequences);


		public:
			//constructor loads the lookup tables
			ThreeCubeSolver();

			//solves the given cube object and returns a vector that contains the twists that were made
			//to solve the cube
			std::vector<cube::Twist> solve(cube::Cube& cube);
	};
}

#endif
