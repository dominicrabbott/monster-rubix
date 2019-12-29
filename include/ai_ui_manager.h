#ifndef AI_UI_MANAGER
#define AI_UI_MANAGER

#include "ui_manager.h"
#include "cube.h"
#include "cube_centers.h"
#include "twist_listener.h"
#include "twist.h"
#include <atomic>

namespace ui{
	class AIUIManager : public UIManager, public ai::TwistListener {
		private:
			//returns true if the ai thread is done finding a solution
			std::atomic<bool> start_solution;

			//returns a random twist object
			cube::Twist random_twist();

			//symbolic representation of the cube used by the AI
			cube::Cube sym_cube;

			//symbolic representation of the centers of the cube used by the AI
			cube::CubeCenters sym_cube_centers;

			//stores the solution to the cube being solved once the solution is found
			std::vector<cube::Twist> solution;

			//starts displaying the solution of the cube once the enter key is pressed
			bool keyPressed(const OgreBites::KeyboardEvent& event);

		public:
			AIUIManager(const int size) : 
				UIManager(size), 
				sym_cube(size), 
				sym_cube_centers(size), 
				start_solution(false) {};

			//has UIManager set the scene and scrambles the cube
			void setup() override;

			void twist(const cube::Twist& twist) override {
				sym_cube_centers.rotate(twist);
				sym_cube.rotate(twist);
				if (start_solution) {
					cube -> rotate(twist);	
				}
				else {
					solution.push_back(twist);
				}
			}

			//solves the cube
			void solve();	
	};
}
#endif
