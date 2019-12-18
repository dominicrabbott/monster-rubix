#ifndef MOVE_LISTENER_H
#define MOVE_LISTENER_H

#include <vector>

namespace cube {
	struct Twist;
}

namespace ai {
	class TwistListener {
		public:
			virtual void twist(const cube::Twist& twist) = 0;
	};
}

#endif
