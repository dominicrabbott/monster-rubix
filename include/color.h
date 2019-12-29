#ifndef COLOR_H
#define COLOR_H

#include <iostream>

namespace ui {
	enum class Color {GREEN, BLUE, YELLOW, WHITE, RED, ORANGE, BLACK};

	std::ostream& operator<<(std::ostream& stream, const Color& color);
}


#endif
