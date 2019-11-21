#ifndef SHOW_UI_MANAGER
#define SHOW_UI_MANAGER

#include "OgreInput.h"
#include "three_cube_solver.h"
#include "ui_manager.h"
#include "cube.h"
#include <boost/optional.hpp>

namespace cube {
	class Twist;
}

namespace ui {
	class ShowUIManager : public UIManager {
		private:
			//once the cube is solved, the series of rotations made to arrive
			//at the solution is stored here
			std::vector<cube::Twist> solution;

			//returns a random twist object
			cube::Twist random_twist();

			//symbolic representation of the cube
			cube::Cube sym_cube;

			//solves when enter is pressed
			bool keyPressed(const OgreBites::KeyboardEvent &event) override;

			//scrambles the cube
			void setup() override;
		public:
			ShowUIManager(): UIManager(3), sym_cube(3) {}

			//solves the cube
			void solve();
				
	};

}

#endif
