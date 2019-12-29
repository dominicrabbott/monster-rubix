#ifndef CUBE_SOLVER_H
#define CUBE_SOLVER_H

#include "twist_listener.h"
#include "twist_provider.h"
#include "cube.h"
#include "cube_centers.h"
#include <boost/optional.hpp>

namespace ai {
	class CubeSolver : public TwistListener, public TwistProvider {
		private:
			boost::optional<cube::Cube> cube;
			boost::optional<cube::CubeCenters> centers;
		public:
			void twist(const cube::Twist& twist) override {
				cube -> rotate(twist);
				centers -> rotate(twist);
				notify_listeners({twist});
			}
			void solve(const cube::Cube& cube, const cube::CubeCenters& centers);

	};
}

#endif
