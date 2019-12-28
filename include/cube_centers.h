#ifndef CUBE_CENTERS_H
#define CUBE_CENTERS_H

#include "face.h"
#include "cube_base.h"
#include <unordered_map>
#include <memory>
#include <array>
#include <boost/optional.hpp>

namespace cube {
	class Twist;
			
	typedef std::array<int, 3> Coords;

	//Optimized symbolic representation of the centers of the cube
	class CubeCenters : public CubeBase {
		private:
			int face_count = cube::ALL_FACES.size();

			//used to track the value of the centers when solved
			//in the cube's current orientation for odd dimensioned cubes
			std::unique_ptr<CubeCenters> solved_center_values;

			//number of pieces in a center
			int center_size;

			//width of center in cartesian space
			uint8_t width_in_cartesian_space;

			//map specifies the fixed coordinates for each face. The first value in the
			//array specifies the index of the coordinate that is fixed, and the other value specifies
			//its value
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

		public:
			//constructs a solved cube of the given size
			CubeCenters(const int size);

			CubeCenters(const CubeCenters& cube);

			CubeCenters& operator=(const CubeCenters& cube);

			//getters for the state of the cube
			int get_center_pos(const Coords coords) const;
			int get_center_pos(const int center) const {return centers[center];}
			int get_fixed_face_coord(const Face face) const {return face_coords.at(face)[0];};
			int get_fixed_face_coord_value(const Face face) const {return face_coords.at(face)[1];};
			int get_solved_center_value(const Face face) const;
			int get_pieces_in_center() const {return center_size;}

			//performs a rotation on the cube
			void rotate(const Twist& twist);

			bool operator==(const CubeCenters& cube) const;
			CubeCenters(CubeCenters&& cube) = default;
	};
}

#endif
