#ifndef TWIST_PROVIDER_H
#define TWIST_PROVIDER_H

#include "twist_listener.h"
#include "twist.h"
#include <vector>

namespace ai {
	class TwistProvider {
		private:
			//vector holds the set of TwistListeners updated when twists are made
			std::vector<TwistListener*> twist_listeners;
		protected:
			//updates the TwistListeners in twist_listeners of the given twists
			void notify_listeners(const std::vector<cube::Twist>& twists) {
				for (const auto& twst : twists) {
					for (const auto listener_ptr : twist_listeners) {
						listener_ptr -> twist(twst);	
					}	
				}
			}
		public:
			void add_twist_listener(TwistListener* listener) {
				twist_listeners.push_back(listener);
			}
	};
}

#endif
