#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <memory>
#include "cube_display.h"
#include "OgreApplicationContext.h"
#include "OgreInput.h"
#include "OgreCameraMan.h"

namespace ui {
	
	class UIManager : public OgreBites::ApplicationContext, public OgreBites::InputListener {
		public:
			UIManager() : OgreBites::ApplicationContext("MonsterRubix") {};
		
		protected:
			//pointer to CubeDisplay object that draws the cube
			Ogre::SceneManager* scene_mgr;

			//pointer to CameraMan object that controls the camera
			std::unique_ptr<OgreBites::CameraMan> camera_man;

			//sets the scene for the rubix cube and initializes the CubeDisplay pointer
			void setup() override;

			//exits when escape is pressed
			bool keyPressed(const OgreBites::KeyboardEvent &event) override;
	};
}

#endif
