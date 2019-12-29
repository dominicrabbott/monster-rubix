#ifndef KEYBOARD_UI_MANAGER
#define KEYBOARD_UI_MANAGER

#include <vector>
#include <regex>
#include <stack>
#include "ui_manager.h"
#include "twist.h"

class string;

namespace ui {

	class KeyboardUIManager : public UIManager {
		private:
			//string made from the keys entered by the user
			std::string keys_pressed;

			//used to track whether shift is pressed
			bool shift_pressed = false;

			//holds previously made twists so moves to the cube can be undone
			std::stack<cube::Twist> prev_moves;

			//when a key is pressed, this function appends the character to the 'keysPressed' string. If the key pressed is 
			//Enter, the input is parsed into a twist that is then made to the cube. Also moves the camera back to 
			//face the front face when Tab is pressed
			bool keyPressed(const OgreBites::KeyboardEvent& event) override;

			//sets shift_pressed to false;
			bool keyReleased(const OgreBites::KeyboardEvent& event) override;

			//makes a rotation to the cube based on the input from the user
			void twist();

			//queries the size of the cube from the user
			int ask_size();
		public:
			KeyboardUIManager() : UIManager(ask_size()) {};
	};
}

#endif
