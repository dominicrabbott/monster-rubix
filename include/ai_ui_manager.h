#ifndef AI_UI_MANAGER
#define AI_UI_MANAGER

#include "ui_manager.h"
#include "combined_cube.h"
#include "twist_listener.h"
#include "twist.h"
#include <atomic>

namespace ui{
	class AIUIManager : public UIManager, public ai::TwistListener {
		private:
			//returns true if the user has started the solution
			std::atomic<bool> start_solution;

			//symbolic representation of the cube
			cube::CombinedCube sym_cube;

			//stores the solution to the cube being solved once the solution is found
			std::vector<cube::Twist> solution;

			//starts displaying the solution of the cube once the enter key is pressed
			bool keyPressed(const OgreBites::KeyboardEvent& event);

		public:
			AIUIManager(const int size) : 
				UIManager(size), 
				sym_cube(size), 
				start_solution(false) {};

			//has UIManager set the scene and scrambles the cube
			void setup() override;

			void twist(const cube::Twist& twist) override {
				sym_cube.rotate(twist);
				if (start_solution) {
					cube -> rotate(twist);	
				}
				else {
					solution.push_back(twist);
				}
			}
			
			void solve();	
	};
}
#endif
