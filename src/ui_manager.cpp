#include "ui_manager.h"
#include "Ogre.h"

using namespace ui;
using namespace Ogre;

void UIManager::setup() {
	ResourceGroupManager::getSingletonPtr() -> addResourceLocation("./resources/images", "FileSystem");
	ResourceGroupManager::getSingletonPtr() -> addResourceLocation("./resources/scripts", "FileSystem");

	OgreBites::ApplicationContext::setup();
	addInputListener(this);

	scene_mgr = getRoot() -> createSceneManager();
	scene_mgr -> setAmbientLight(ColourValue(1,1,1));

	auto* cam = scene_mgr -> createCamera("main_cam");
	cam -> setAutoAspectRatio(true);
	auto* cam_node = scene_mgr -> getRootSceneNode() -> createChildSceneNode();
	cam_node -> attachObject(cam);
	cam_node -> setPosition(0, 1000, 500);
	camera_man = std::make_unique<OgreBites::CameraMan>(cam_node);
	camera_man -> setStyle(OgreBites::CameraStyle::CS_ORBIT);
	addInputListener(camera_man.get());

	auto* viewport = getRenderWindow() -> addViewport(cam);
	viewport -> setBackgroundColour(ColourValue(.95,.95,.85));
}

bool UIManager::keyPressed(const OgreBites::KeyboardEvent &event) {
	if (event.keysym.sym == OgreBites::SDLK_ESCAPE) {
		getRoot() -> queueEndRendering();	
	}

	return true;
}
