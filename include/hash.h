#ifndef HASH_H
#define HASH_H

#include <boost/functional/hash.hpp>
#include <algorithm>
#include <array>
#include "cube_centers.h"
#include "twist.h"

namespace std {
	template<typename T> 
	struct hash<std::vector<T>> {
		size_t operator()(const std::vector<T> vec) const {
			return boost::hash_range(vec.begin(), vec.end());
		}
	};
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
			size_t seed;
			for (int i = 0; i < std::pow(centers.get_size()-2, 2)*6; i++) {
				boost::hash_combine(seed, centers.get_center_pos(i));	
			}

			return seed;
		}	
	};
}

#endif
