#ifndef CUBE_BASE_H
#define CUBE_BASE_H

#include <unordered_map>
#include <array>
#include <memory>
#include "face.h"

namespace cube {
	class Twist;

	//Optimized symbolic representation of the centers of the cube
	class CubeBase {
		protected:
			//the size of the cube
			uint8_t size;

			//width of an edge, excluding the corners 
			uint8_t edge_width;

			//circular-shifts the pieces in the given array located at the given indecies.
			//A 90 degree rotation causes 1 circular shift, and a -90 degree rotation 3 circular
			//shifts. The offset value is added to each of the indecies, making it easier to 
			//rotate the nth edge or the nth center, where n != 1
			void shift_pieces(uint8_t* pieces, const std::array<int, 4> indecies, const int degrees);
		
		public:
			//constructs a solved cube of the given size
			CubeBase(const int size);

			CubeBase(const CubeBase& cube);

			//getters for the state of the cube
			int get_size() const {return size;}
			int get_edge_width() const {return edge_width;}
	};
}

#endif
