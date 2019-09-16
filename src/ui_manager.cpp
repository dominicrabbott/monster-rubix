#include <iostream>

#include "Ogre.h"
#include "ui_manager.h"

using namespace ui;
using namespace Ogre;

UIManager::UIManager() : OgreBites::ApplicationContext("RubixII") {
	std::cout << "Enter the size of the cube\n";
	std::cin >> size;
}

void UIManager::setup() {
	ResourceGroupManager::getSingletonPtr() -> addResourceLocation("./resources/images", "FileSystem");
	ResourceGroupManager::getSingletonPtr() -> addResourceLocation("./resources/scripts", "FileSystem");

	OgreBites::ApplicationContext::setup();
	addInputListener(this);

	SceneManager *scene_mgr = getRoot() -> createSceneManager();

	scene_mgr -> setAmbientLight(ColourValue(1,1,1));

	Camera* cam = scene_mgr -> createCamera("main_cam");
	cam -> setAutoAspectRatio(true);
	SceneNode* cam_node = scene_mgr -> getRootSceneNode() -> createChildSceneNode();
	cam_node -> attachObject(cam);
	cam_node -> setPosition(0, 1000, 500);
	camera_man = std::make_unique<OgreBites::CameraMan>(cam_node);
	camera_man -> setStyle(OgreBites::CameraStyle::CS_ORBIT);
	addInputListener(camera_man.get());

	Viewport* viewport = getRenderWindow() -> addViewport(cam);
	viewport -> setBackgroundColour(ColourValue(.95,.95,.85));

	cube = std::make_unique<CubeDisplay>(size, scene_mgr);
	addInputListener(cube.get());
}

bool UIManager::keyPressed(const OgreBites::KeyboardEvent &event) {
	if (event.keysym.sym == OgreBites::SDLK_ESCAPE) {
		getRoot() -> queueEndRendering();	
	}

	return true;
}
