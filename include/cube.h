#ifndef CUBE_H
#define CUBE_H

#include <unordered_map>
#include <memory>
#include "face.h"

int main();

namespace cube {
	class Twist;

	//Optimized symbolic representation of the cube adapted from http://www.cube20.org/src/cubepos.pdf
	class Cube {
		private:
			//numbers the faces of the cube
			static std::unordered_map<Face, int> face_numbers;
			//specifies the faces that is opposite each face
			static std::unordered_map<Face, Face> opposing_faces;

			//the size of the cube
			const unsigned char size;

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
			void rotate_corner(const int corner);

			//transposes the matrix that is made of the centers of a face
			void transpose_center(const Face face);

			//reverses the rows in the matrix that is made of the centers of the face
			void reverse_center_rows(const Face face);

			//performs a 90 degree rotation on the outermost layer of the given face
			void rotate_face(const Face face);

			//performs a 90 degree rotation on the specified slice of the cube
			void rotate_slice(const Face face, const int layer);

		public:
			//constructs a solved cube of the given size
			Cube(const int size);

			//getters for the state of the cube
			int get_size() {return size;}
			int get_center_pos(const int center) const {return centers[center];}
			int get_edge_pos(const int edge) const {return edges[edge] & 0x7F;}
			int get_corner_pos(const int corner) const {return corners[corner] & 7;}
			int get_edge_orientation(const int edge) const {return edges[edge] >> 7;}
			int get_corner_orientation(const int corner) const {return corners[corner] >> 3;}

			//performs a rotation on the cube
			void rotate(const Twist& twist);

			//function to allow for the object to be placed in a unordered_set
			bool operator==(const Cube& cube) const	{
				return centers == cube.centers && edges == cube.edges && corners == cube.corners;
			}
	};
}

#endif
