#ifndef CUBE_DISPLAY_H
#define CUBE_DISPLAY_H

#include <vector>
#include <deque>
#include <unordered_map>

#include "Ogre.h"
#include "OgreInput.h"
#include "face.h"
#include "cube.h"
#include "twist.h"

namespace cube {
	class Piece;
}

namespace ui {

	class CubeDisplay : public OgreBites::InputListener {
		private:
			//used to represent rotations to the drawn cube. This struct is more optimal for
			//representing rotations in 3d space than the Twist class, which is better for
			//symbolic manipulation of the cube
			struct Rotation {
				cube::Face axis;
				int total_degrees;
				int remaining_degrees;
				int from_layer;
				int to_layer;

				Rotation(const cube::Twist& move, const int cube_size);
			};

			Ogre::SceneManager* scene_mgr;
			
			//size of each piece of the cube	
			const int piece_size = 100;

			//rotations that are to be made to the cube
			std::deque<Rotation> pending_rotations;

			//Each vector in this set makes up one axis of the cube.
			//Each axis is made up of several SceneNodes in a line, each located at the center of
			//a face. Cube entities are positioned relative to these SceneNodes and the SceneNodes
			//are rotated to rotate a face
			std::unordered_map<cube::Face, std::vector<Ogre::SceneNode*>> skeleton;

			//an array of SceneNode pointers to which entities are 
			//attached that make up the pieces of the cube
			std::vector<std::vector<std::vector<Ogre::SceneNode*>>> cube;

			//populate the skeleton set as described above
			void create_skeleton(const int size);
			
			//returns a pointer to a SceneNode that represents the given piece
			Ogre::SceneNode* create_piece(const cube::Piece& piece);

			//when there is an ongoing rotation, thus function rotates the cube
			//before every frame accordingly
			void frameRendered(const Ogre::FrameEvent& event) override;

			//creates the SceneNodes that represent the pieces in the cube and populates the 'cube'
			//member variable with them
			void create_cube(const int size);

			//transfer a SceneNode from its current parent to another SceneNode
			void adopt_scene_node(Ogre::SceneNode* child, Ogre::SceneNode* new_parent);

		public:
			//creates a CubeDisplay object that starts by displaying an unscrambled cube of dimensions 
			//size x size x size with the given SceneManager pointer. The displayed cube can then be 
			//manipulated by the CubeDisplay::rotate function 
			CubeDisplay(const int size, Ogre::SceneManager* scene_mgr);

			//performs the given rotation
			void rotate(const cube::Twist &move);
	
	};
}

#endif
