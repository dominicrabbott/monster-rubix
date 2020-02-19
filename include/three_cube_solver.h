#ifndef THREE_CUBE_SOLVER
#define THREE_CUBE_SOLVER

#include <boost/bimap.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <boost/filesystem.hpp>
#include <algorithm>
#include <string>
#include <unordered_set>
#include <functional>
#include <memory>
#include <array>
#include "twist.h"
#include "hash.h"
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
			typedef boost::bimap<boost::bimaps::unordered_set_of<cube::Twist, std::hash<cube::Twist>>, char> TwistEncodingMap;
			static constexpr int stage_count = 4;
			
			const static ptrdiff_t cube_encoding_length = 11;
			const static ptrdiff_t twists_encoding_length = 30;
			const static ptrdiff_t table_entry_length = cube_encoding_length+twists_encoding_length;

			//maps the 90 and -90 degree rotations of every face to an 8-bit integer encoding
			TwistEncodingMap twist_mappings;

			std::array<std::string, stage_count> filenames = {
				"g1_table.bin",
				"g2_table.bin",
				"g3_table.bin",
				"g4_table.bin",
			};

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
			boost::filesystem::path table_dir;

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

			//used to populate the twist_mappings member variable
			TwistEncodingMap create_twist_mappings();

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
			//to the root state
			std::vector<cube::Twist> get_twists(const State* state);

			//creates a vector that contains the twist sequences that can be made at a stage. If a face
			//exists in the restricted_faces parameter, only 180 degree turns of that face are allowed. Otherwise, 90 and -90 degree
			//turns are allowed
			std::vector<TwistSequence> generate_twist_sequences(const std::unordered_set<cube::Face>& restricted_faces);

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

			//converts an encoding to a form that makes saving the encoding
			//to disk easier
			std::vector<char> convert_encoding(const std::vector<bool>& encoding);
			
			//creates a lookup table of the twists needed move a cube from one stage to the next stage. The allowed twist sequences
			//for the stage and the encoder function for the stage are taken as paramaters
			LookupTable create_lookup_table(const Encoder encoder, const std::vector<TwistSequence> twist_sequences);

			//performs a binary search on the entries in the given memory-mapped lookup table file to find the twists needed
			//to solve the cube represented by 'target_encoding'
			std::vector<cube::Twist> find_table_entry(const std::vector<bool>& target_encoding, const boost::iostreams::mapped_file_source& table);

			//notifies the twist listeners of an object of the twists contained in 'twist_sequence'
			//and performs those twists on 'comb_cube'
			void execute_partial_solution(const TwistSequence& twist_sequence, cube::CombinedCube& comb_cube);

		public:
			//constructor generates the lookup tables if they are not already generated
			ThreeCubeSolver();

			//solves the given cube object and notifies any twist listeners of the twists 
			//found to solve the cube
			void solve(const cube::CombinedCube& comb_cube);
	};
}

#endif
