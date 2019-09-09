#include <ostream>
#include "face.h"

using namespace cube;

std::ostream& cube::operator<<(std::ostream& stream, const Face& face) {
	switch (face) {
		case Face::RIGHT:
			stream << "R";
			break;
		case Face::LEFT:
			stream << "L";
			break;
		case Face::TOP:
			stream << "T";
			break;
		case Face::BOTTOM:
			stream << "D";
			break;
		case Face::FRONT:
			stream << "F";
			break;
		case Face::BACK:
			stream << "B";
			break;
	};

	return stream;
}
