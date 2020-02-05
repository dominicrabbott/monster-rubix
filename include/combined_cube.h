#ifndef COMBINED_CUBE_H
#define COMBINED_CUBE_H

#include "cube.h"
#include "cube_centers.h"
#include "twist.h"

namespace cube {

	class CombinedCube {
		private:
			Cube cube;
			CubeCenters cube_centers;

		public:
			CombinedCube(const int size) : cube(size), cube_centers(size) {}

			Cube& get_cube() {
				return cube;
			}

			CubeCenters& get_cube_centers() {
				return cube_centers;	
			}
			
			const Cube& get_cube() const {
				return cube;
			}

			const CubeCenters& get_cube_centers() const {
				return cube_centers;	
			}


			void rotate(const Twist& twist) {
				cube.rotate(twist);
				cube_centers.rotate(twist);	
			}

			int get_size() {
				return cube.get_size();
			}

			bool operator==(const CombinedCube& comb_cube) const {
				return comb_cube.get_cube()==cube && comb_cube.get_cube_centers()==cube_centers;	
			}
	};
}

#endif
