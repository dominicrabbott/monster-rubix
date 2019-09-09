#ifndef FACE_H
#define FACE_H

namespace cube {
	enum class Face {RIGHT, LEFT, TOP, BOTTOM, FRONT, BACK};
	class ostream;
	
	std::ostream& operator<<(std::ostream& stream, const Face& face);
}

#endif
