#ifndef FACE_H
#define FACE_H
	
class ostream;

namespace cube {
	enum class Face {RIGHT, LEFT, TOP, BOTTOM, FRONT, BACK};

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
