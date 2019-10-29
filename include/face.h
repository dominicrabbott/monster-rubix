#ifndef FACE_H
#define FACE_H
	
class ostream;

namespace cube {
	enum class Face : int {LEFT = 0, TOP = 1, BACK = 2, BOTTOM = 3, FRONT = 4, RIGHT = 5};

	const Face ALL_FACES[] = {
		Face::RIGHT,	
		Face::LEFT,	
		Face::TOP,	
		Face::BOTTOM,	
		Face::FRONT,	
		Face::BACK,
	};
	
	std::ostream& operator<<(std::ostream& stream, const Face& face);
}

#endif
