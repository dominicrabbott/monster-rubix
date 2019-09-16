#include "ui_manager.h"
#include "Ogre.h"

int main() {
	ui::UIManager man;
	man.initApp();
	man.getRoot() -> startRendering();
	man.closeApp();
}
