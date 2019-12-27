#ifndef CUBE_H
#define CUBE_H

#include <unordered_map>
#include <memory>

#include "face.h"
#include "cube_base.h"

namespace cube {
	class Twist;

	//Optimized symbolic representation of the cube,
	//exculding the centers, adapted from http://www.cube20.org/src/cubepos.pdf
	class Cube : public CubeBase {
		private:
			//first 7 bits specify position, last bit specifies orientation
			//orientation bit is flipped on every rotation
			std::unique_ptr<uint8_t[]> edges;

			//first 3 bits specify position, list 2 bits specify orientation
			//
			//orientation encoding is as follows:
			//	0 - correct orientation
			//	1 - clockwise twist
			//	2 - counter-clockwise twist
			std::unique_ptr<uint8_t[]> corners;

			//helper functions for manipulating the pieces
			
			//flips the orientation of an edge if the given face is Top or Bottom
			void flip_edge(const int edge, const cube::Face face) {
				if (face == cube::Face::TOP || face == cube::Face::BOTTOM) {
					edges[edge] ^= 0x80;
				}
			}

			//flips the orientation of an edge unconditionally
			void flip_edge(const int edge) {
				edges[edge] ^= 0x80;	
			}

			//rotates a corner clockwise
			//
			//a rotation of 2 performs a counter-clockwise twist, and a rotation
			//of 1 a clockwise twist
			void rotate_corner(const int corner, const int rotation) {
				int orientation = (get_corner_orientation(corner)+rotation)%3;
				corners[corner] &= 0xE7;
				corners[corner] |= orientation << 3;	
			}

			//performs a 90 degree rotation on the outermost layer of the given face
			void rotate_face(const Face face, const int degrees);

			//performs a 90 degree rotation on the specified slice of the cube
			void rotate_slice(const Face face, const int layer, const int degrees);

			//copies an array of pieces and returns a unique_ptr to the data
			std::unique_ptr<uint8_t[]> copy_pieces(const uint8_t* array, const int size); 

		public:
			//constructs a solved cube of the given size
			Cube(const int size);

			Cube(const Cube& cube);

			Cube& operator=(const Cube& cube);

			//getters for the state of the cube
			int get_edge_pos(const int edge) const {return edges[edge] & 0x7F;}
			int get_corner_pos(const int corner) const {return corners[corner] & 7;}
			int get_edge_orientation(const int edge) const {return edges[edge] >> 7;}
			int get_corner_orientation(const int corner) const {return corners[corner] >> 3;}
			int get_edge_count() const {return 12;}
			int get_corner_count() const {return 8;};

			//performs a rotation on the cube
			void rotate(const Twist& twist);

			bool operator==(const Cube& cube) const;
			Cube(Cube&& cube) = default;
	};
}

#endif
