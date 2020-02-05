#ifndef SCRAMBLE_GENERATOR_H
#define SCRAMBLE_GENERATOR_H

#include <vector>
#include <cstdlib>
#include "twist.h"
#include "face.h"

namespace cube {
	namespace scramble_generator {
		inline std::vector<cube::Twist> generate_scramble(const int scramble_length, const int cube_size, const int seed=std::time(nullptr)) {
			std::vector<cube::Twist> scramble_twists;
			std::srand(seed);
			for (int i = 0; i < scramble_length; i++) {
				int degrees = std::rand()%2 ? 90 : -90;
				Face face = cube::ALL_FACES[std::rand()%6];
				int layer = std::rand() % (cube_size/2);
				bool wide_turn = std::rand()%2;

				scramble_twists.push_back(cube::Twist(degrees, face, layer, wide_turn));
			}

			return scramble_twists;
		}
	}
}

#endif
