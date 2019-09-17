#ifndef KEYBOARD_UI_MANAGER
#define KEYBOARD_UI_MANAGER

#include <vector>
#include <regex>

#include "ui_manager.h"

namespace cube {
	class Twist;
}

class string;

namespace ui {

	class KeyboardUIManager : public UIManager {
		private:
			//string made from the keys entered by the user
			std::string keys_pressed;

			//used to track whether shift is pressed
			bool shift_pressed = false;

			//when a key is pressed, this function appends the character to the 'keysPressed' string. If the key pressed is 
			//Enter, the input is parsed into a twist that is then made to the cube. Also moves the camera back to 
			//face the front face when Tab is pressed
			bool keyPressed(const OgreBites::KeyboardEvent& event) override;

			//sets shift_pressed to false;
			bool keyReleased(const OgreBites::KeyboardEvent& event) override;

			//makes a rotation to the cube based on the input from the user
			void twist();
	};
}

#endif
