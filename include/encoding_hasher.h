#include <boost/functional/hash.hpp>
#include <algorithm>

namespace std {
	template<>
	struct hash<std::vector<uint8_t>> {
		size_t operator()(const std::vector<uint8_t> vec) const {
			size_t seed = 0;
			boost::hash_range(seed, vec.begin(), vec.end());

			return seed;
		}	
	};

	inline bool operator==(const std::vector<uint8_t> vec1, const std::vector<uint8_t> vec2) {
		return std::equal(vec1.begin(), vec1.end(), vec2.begin());	
	}

}
