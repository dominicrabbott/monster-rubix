#ifndef CUBE_H
#define CUBE_H

#include <unordered_map>
#include <memory>
#include "face.h"

namespace cube {
	class Twist;

	//Optimized symbolic representation of the cube adapted from http://www.cube20.org/src/cubepos.pdf
	class Cube {
		private:
			//numbers the faces of the cube
			static std::unordered_map<Face, int> face_numbers;

			//the size of the cube
			unsigned char size;

			//width of an edge
			unsigned char edge_width;

			//number of pieces in a center
			unsigned char center_size;

			//the stored representation of the pieces
			//
			//all 8 bits specify position
			std::unique_ptr<unsigned char[]> centers;

			//first 7 bits specify position, last bit specifies orientation
			//orientation bit is flipped on every rotation
			std::unique_ptr<unsigned char[]> edges;

			//first 3 bits specify position, list 2 bits specify orientation
			//
			//orientation encoding is as follows:
			//	0 - correct orientation
			//	1 - clockwise twist
			//	2 - counter-clockwise twist
			std::unique_ptr<unsigned char[]> corners;

			//helper functions for manipulating the pieces
			
			//flips the orientation of an edge
			void flip_edge(const int edge) {edges[edge] ^= 0x80;}

			//rotates a corner clockwise
			//
			//a rotation of 2 performs a counter-clockwise twist, and a rotation
			//of 1 a clockwise twist
			void rotate_corner(const int corner, const int rotation) {
				int orientation = (get_corner_orientation(corner)+rotation)%3;
				corners[corner] &= 0xE7;
				corners[corner] |= orientation << 3;	
			}

			//transposes the matrix that is made of the centers of a face
			void transpose_center(const Face face);

			//reverses the rows in the matrix that is made of the centers of the face
			void reverse_center_rows(const Face face);

			//circular-shifts the pieces in the given array located at the given indecies.
			//A 90 degree rotation causes 1 circular shift, and a -90 degree rotation 3 circular
			//shifts. The offset value is added to each of the indecies, making it easier to 
			//rotate the nth edge or the nth center, where n != 1
			void shift_pieces(unsigned char* pieces, int* indecies, int degrees, int offset);

			//performs a 90 degree rotation on the outermost layer of the given face
			void rotate_face(const Face face, const int degrees);

			//performs a 90 degree rotation on the specified slice of the cube
			void rotate_slice(const Face face, const int layer, const int degrees);

			//copies an array of pieces and returns a unique_ptr to the data
			std::unique_ptr<unsigned char[]> copy_pieces(const unsigned char* array, const int size); 

		public:
			//constructs a solved cube of the given size
			Cube(const int size);

			Cube(const Cube& cube);

			Cube& operator=(const Cube& cube);

			//getters for the state of the cube
			int get_size() const {return size;}
			int get_center_pos(const int center) const {return centers[center];}
			int get_edge_pos(const int edge) const {return edges[edge] & 0x7F;}
			int get_corner_pos(const int corner) const {return corners[corner] & 7;}
			int get_edge_orientation(const int edge) const {return edges[edge] >> 7;}
			int get_corner_orientation(const int corner) const {return corners[corner] >> 3;}

			//performs a rotation on the cube
			void rotate(const Twist& twist);

			bool operator==(const Cube& cube) const;
			Cube(Cube&& cube) = default;

			friend std::hash<Cube>;
	};
}

namespace std {
	template<>
	struct hash<cube::Cube> {
		size_t operator()(const cube::Cube& cube) const;
	};
}

#endif
