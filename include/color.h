#ifndef COLOR_H
#define COLOR_H

class ostream;

namespace ui {
	enum class Color {GREEN, BLUE, YELLOW, WHITE, RED, ORANGE, BLACK};

	std::ostream& operator<<(std::ostream& stream, const Color& color);
}


#endif
