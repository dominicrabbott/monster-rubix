#ifndef CUBE_SOLVER_H
#define CUBE_SOLVER_H

#include "twist_listener.h"
#include "twist_provider.h"
#include "combined_cube.h"
#include <boost/optional.hpp>

namespace ai {
	class CubeSolver : public TwistListener, public TwistProvider {
		private:
			boost::optional<cube::CombinedCube> comb_cube;
		public:
			void twist(const cube::Twist& twist) override {
				comb_cube.get().rotate(twist);
				notify_listeners({twist});
			}
			void solve(const cube::CombinedCube& comb_cube);

	};
}

#endif
