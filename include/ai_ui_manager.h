#ifndef AI_UI_MANAGER
#define AI_UI_MANAGER

#include "ui_manager.h"
#include "cube.h"
#include "cube_centers.h"
#include "twist_listener.h"

namespace cube {
	struct Twist;
}

namespace ui{
	class AIUIManager : public UIManager, public ai::TwistListener {
		private:
			//returns a random twist object
			cube::Twist random_twist();

			//symbolic representation of the cube used by the AI
			cube::Cube sym_cube;

			//symbolic representation of the centers of the cube used by the AI
			cube::CubeCenters sym_cube_centers;

		public:
			AIUIManager(const int size) : UIManager(size), sym_cube(size), sym_cube_centers(size) {};

			//has UIManager set the scene and scrambles the cube
			void setup() override;

			//makes the given twists to the UI
			void twist(const cube::Twist& twist) override {
				cube->rotate(twist);	
			}

			//solves the cube
			void solve();	
	};
}
#endif
