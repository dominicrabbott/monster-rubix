#ifndef AI_UI_MANAGER
#define AI_UI_MANAGER

#include "ui_manager.h"
#include "cube.h"

namespace cube {
	class Twist;
}

namespace ui{
	class AIUIManager : public UIManager {
		private:
			//returns a random twist object
			cube::Twist random_twist();

			//symbolic representation of the cube used by the AI
			cube::Cube sym_cube;
		public:
			AIUIManager(const int size) : UIManager(size), sym_cube(cube::Cube(size)) {};

			//has UIManager set the scene and scrambles the cube
			void setup() override;

			//solves the cube
			void solve();	
	};
}
#endif
