#include "color.h"

using namespace ui;

std::ostream& ui::operator<<(std::ostream& stream, const Color& color) {
	switch (color) {
		case Color::GREEN:
			stream << "Green";
			break;
		case Color::BLUE:
			stream << "Blue";
			break;
		case Color::YELLOW:
			stream << "Yellow";
			break;
		case Color::WHITE:
			stream << "White";
			break;
		case Color::RED:
			stream << "Red";
			break;
		case Color::ORANGE:
			stream << "Orange";
			break;
		default:
			stream << "Black";
	};

	return stream;
}
