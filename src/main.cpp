#include "keyboard_ui_manager.h"
#include "Ogre.h"

int main() {
	ui::KeyboardUIManager man;
	man.initApp();
	man.getRoot() -> startRendering();
	man.closeApp();
}
