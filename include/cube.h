#ifndef CUBE_H
#define CUBE_H

#include <vector>
#include "piece.h"

namespace cube {
	class Twist;
	class Piece;
	enum class Color;
	enum class Face;

	//represents an NxNxN cube, where N >= 3
	class Cube {
		private:
			std::vector<std::vector<std::vector<Piece>>> cube;
		public:
			//creates an unscrambled (solved) cube object
			Cube(const int size);

			//rotates the cube according to the given Twist object
			void rotate(const Twist& move);

			//returns the piece located at the indicated position on the cube
			const Piece& get_piece(int x, int y, int z) const {return cube[x][y][z];}

			//retuns the colors that are present on the given face
			std::vector<std::vector<Color>> get_face(const Face face);

	};
}

#endif
