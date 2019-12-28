#ifndef HASH_H
#define HASH_H

#include <boost/functional/hash.hpp>
#include <algorithm>
#include <array>
#include "cube_centers.h"
#include "cube.h"
#include "twist.h"

namespace std {
	template<typename T, size_t size> 
	struct hash<std::array<T, size>> {
		size_t operator()(const std::array<T, size> arr) const {
			return boost::hash_range(arr.begin(), arr.end());
		}
	};

	template<>
	struct hash<cube::Twist> {
		size_t operator()(const cube::Twist& twist) const {
			size_t seed = 0;
			boost::hash_combine(seed, twist.degrees);
			boost::hash_combine(seed, twist.face);
			boost::hash_combine(seed, twist.wide_turn);
			boost::hash_combine(seed, twist.layer);

			return seed;
		}
	};
	
	template<>
	struct hash<cube::CubeCenters> {
		size_t operator()(const cube::CubeCenters& centers) const {
			size_t seed = 0;
			for (int i = 0; i < std::pow(centers.get_size()-2, 2)*6; i++) {
				boost::hash_combine(seed, centers.get_center_pos(i));	
			}

			return seed;
		}	
	};
	
	template <>
	struct hash<cube::Cube> {
		size_t operator()(const cube::Cube& cube) const {
			size_t seed = 0;
			int edge_count = 12*(cube.get_size()-2);
			int corner_count = 8;
			for (int i = 0; i < edge_count; i++) {
				boost::hash_combine(seed, cube.get_edge_pos(i));	
				boost::hash_combine(seed, cube.get_edge_orientation(i));	
			}
			for (int i = 0; i < corner_count; i++) {
				boost::hash_combine(seed, cube.get_corner_pos(i));	
				boost::hash_combine(seed, cube.get_corner_orientation(i));	
			}

			return seed;
		}
	};
}

#endif
