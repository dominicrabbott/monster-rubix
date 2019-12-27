#ifndef SHOW_UI_MANAGER
#define SHOW_UI_MANAGER

#include "OgreInput.h"
#include "three_cube_solver.h"
#include "twist_listener.h"
#include "ui_manager.h"
#include "cube.h"
#include "twist.h"
#include <boost/optional.hpp>

namespace ui {
	class ShowUIManager : public UIManager, public ai::TwistListener {
		private:
			//stores the series of rotations made to solve the cube once a solution
			//is found
			std::vector<cube::Twist> solution;

			cube::Twist random_twist();

			cube::Cube sym_cube;

			//solves when enter is pressed
			bool keyPressed(const OgreBites::KeyboardEvent &event) override;

			//scrambles the cube
			void setup() override;

			void twist(const cube::Twist& twist) override {
				solution.push_back(twist);	
			}
		public:
			ShowUIManager(): UIManager(3), sym_cube(3) {}

			void solve();
				
	};

}

#endif
