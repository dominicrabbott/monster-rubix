#ifndef PIECE_H
#define PIECE_H

#include <unordered_map>

namespace cube {
	enum class Face;
	enum class Color;

	//represents a piece in a rubix cube. An NxNxN rubix cube is made of N^3 individual pieces
	class Piece {
		private:
			std::unordered_map<Face, Color> faces;
			static std::unordered_map<Face, std::vector<Face>> adjacent_faces;
		public:
			//returns the color of the given face
			Color get_face(const Face face) const;

			//sets the given face to the given color
			void set_face(const Face face, const Color color) {faces.insert(std::make_pair(face, color));}

			//performs the given rotation on this Piece with respect to the given face
			void rotate(const int degrees, const Face face);
	
	};

}

#endif
