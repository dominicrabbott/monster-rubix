#include <string>
#include <iostream>
#include <unordered_map>

#include "keyboard_ui_manager.h"
#include "face.h"

using namespace ui;
using namespace Ogre;

bool KeyboardUIManager::keyPressed(const OgreBites::KeyboardEvent& event) {
	UIManager::keyPressed(event);

	int ENTER = 13;
	int TAB = 9;

	if (event.keysym.sym == ENTER) {
		twist();
		keys_pressed = "";	
	}

	else if (event.keysym.sym == TAB) {
		camera_man -> getCamera() -> setPosition(0,500,1000);
		camera_man -> getCamera() -> lookAt(Vector3(0,0,0), Node::TransformSpace::TS_WORLD);
	}

	else if (event.keysym.sym == 'c') {
		keys_pressed = "";	
	}

	else if (event.keysym.sym == 'u') {
		if (prev_moves.size() > 0) {
			cube::Twist& move = prev_moves.top();
			cube -> rotate(cube::Twist(-move.degrees, move.face, move.layer, move.wide_turn));
			prev_moves.pop();
		}
	}


	else if (event.keysym.sym == OgreBites::SDLK_LSHIFT) {
		shift_pressed = true;	
	}

	else {
		if (shift_pressed) {
			keys_pressed += std::toupper(event.keysym.sym);	
		}
		else {
			keys_pressed += event.keysym.sym;	
		}
	}
	
	return true;
}

bool KeyboardUIManager::keyReleased(const OgreBites::KeyboardEvent& event) {
	if (event.keysym.sym == OgreBites::SDLK_LSHIFT) {
		shift_pressed = false;
	}

	return true;
}


void KeyboardUIManager::twist() {
	static std::string faces = "([RrLlDdTtFfBb])";
	static std::regex form1(faces);
	static std::regex form2("(\\d+)" + faces + "(w?)");

	char face_char;
	std::string layer_str;
	std::smatch matches;
	
	cube::Face face;
	int degrees;
	int layer;
	bool wide_turn;
	
	bool valid = false;

	if (std::regex_match(keys_pressed, matches, form1)) {
		layer_str = "1";
		face_char = static_cast<std::string>(matches[1])[0];
		wide_turn = false;
		valid = true;
	}

	else if (std::regex_match(keys_pressed, matches, form2)) {
		layer_str = matches[1];
		face_char = static_cast<std::string>(matches[2])[0];
		wide_turn = matches[matches.length()] == "w" ? true : false;
		valid = true;
	}

	if (valid) {	
		static std::unordered_map<char, cube::Face> notation_map = {
			{'R', cube::Face::RIGHT},
			{'L', cube::Face::LEFT},
			{'T', cube::Face::TOP},
			{'D', cube::Face::BOTTOM},
			{'F', cube::Face::FRONT},
			{'B', cube::Face::BACK},	
		};

		if (notation_map.count(face_char) == 0) {
			degrees = -90;
			face = notation_map[std::toupper(face_char)];
		}
		else {
			degrees = 90;
			face = notation_map[face_char];	
		}

		layer = std::stoi(layer_str) - 1;

		if (layer < size) {
			cube::Twist move(degrees, face, layer, wide_turn);

			prev_moves.push(move);
			cube -> rotate(move);
		}
	}
}

int KeyboardUIManager::ask_size() {
	std::cout << "Enter the size of the cube\n";

	int size;
	std::cin >> size;

	return size;
}
