#ifndef CUBE_CENTERS_H
#define CUBE_CENTERS_H

#include <unordered_map>
#include <memory>
#include <array>

#include "face.h"
#include "cube_base.h"

namespace cube {
	class Twist;

	//Optimized symbolic representation of the centers of the cube
	class CubeCenters : public CubeBase {
		private:
			//number of pieces in a center
			uint8_t center_size;

			//map specifies the fixed coordinates for each face
			std::unordered_map<Face, std::array<int, 2>> face_coords = {
				{Face::FRONT, {2,edge_width+1}},	
				{Face::BACK, {2,0}},	
				{Face::RIGHT, {0,edge_width+1}},	
				{Face::LEFT, {0,0}},	
				{Face::TOP, {1,edge_width+1}},	
				{Face::BOTTOM, {1,0}},	
			};

			//the stored representation of the pieces
			//
			//all 8 bits specify position
			std::unique_ptr<uint8_t[]> centers;
			
			//helper functions for manipulating the pieces
			//
			//
			//transposes the matrix that is made of the centers of a face
			void transpose_center(const Face face);

			//reverses the rows in the matrix that is made of the centers of the face
			void reverse_center_rows(const Face face);

			//performs a 90 degree rotation on the outermost layer of the given face
			void rotate_face(const Face face, const int degrees);

			//performs a 90 degree rotation on the specified slice of the cube
			void rotate_slice(const Face slice_face, const int layer, const int degrees);

			//copies an array of pieces and returns a unique_ptr to the data
			std::unique_ptr<uint8_t[]> copy_pieces(const uint8_t* array, const int size); 

		public:
			//constructs a solved cube of the given size
			CubeCenters(const int size);

			CubeCenters(const CubeCenters& cube);

			CubeCenters& operator=(const CubeCenters& cube);

			//getters for the state of the cube
			int get_center_pos(const int center) const {return centers[center];}

			//performs a rotation on the cube
			void rotate(const Twist& twist);

			bool operator==(const CubeCenters& cube) const;
			CubeCenters(CubeCenters&& cube) = default;
	};
}

#endif
