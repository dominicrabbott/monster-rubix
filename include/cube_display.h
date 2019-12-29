#ifndef CUBE_DISPLAY_H
#define CUBE_DISPLAY_H

#include "Ogre.h"
#include "OgreInput.h"
#include "face.h"
#include "twist.h"
#include "color.h"
#include <vector>
#include <queue>
#include <unordered_map>
#include <atomic>
#include <cassert>

class mutex;

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

			//number of degrees to rotate each frame
			std::atomic<int> frames_per_rotation;

			//mutex used to enable the class to be thread-safe
			std::mutex mex;

			//pointer to the SceneManager that this class will
			//use to draw a cube
			Ogre::SceneManager* scene_mgr;
			
			//size of each piece of the cube	
			const int piece_size = 100;

			//rotations that are to be made to the cube
			std::queue<Rotation> pending_rotations;

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
			Ogre::SceneNode* create_piece(const std::unordered_map<cube::Face, Color>& face_colors);

			//returns a map that represents the colors present on the piece of a cube located at
			//the specified coordinates
			std::unordered_map<cube::Face, Color> create_piece_colors(const Ogre::Vector3 coords, const int size);

			//when there is an ongoing rotation, thus function rotates the cube
			//before every frame accordingly
			void frameRendered(const Ogre::FrameEvent& event) override;

			//creates the SceneNodes that represent the pieces in the cube and populates the 'cube'
			//member variable with them
			void create_cube(const int size);

			//transfer a SceneNode from its current parent to another SceneNode
			void adopt_scene_node(Ogre::SceneNode* child, Ogre::SceneNode* new_parent);

			//Helper functions for maintaining the position of the pieces within the cube as they
			//are moved around the various layers of the cube
			//
			//
			//transposes the given layer. Pointers to the nodes to be transposed are accepted as arguments
			void transpose_layer(const std::vector<std::vector<Ogre::SceneNode**>>& layer);
			
			//reverses the rows in the given layer. Pointers to the elements to be 
			//reversed are accepted as arguments
			void reverse_layer_rows(const std::vector<std::vector<Ogre::SceneNode**>>& matrix);
			
			//performs a rotation to the given layer. Pointers to the nodes that are to be
			//rotated are accepted as arguments
			void rotate_layer(const std::vector<std::vector<Ogre::SceneNode**>>& layer, const int degrees);

			//returns a matrix that contains pointers to the elements that make up the specified 
			//layer in the given cube
			std::vector<std::vector<Ogre::SceneNode**>> find_layer(const cube::Face face, const int layer);

		public:
			//creates a CubeDisplay object that starts by displaying an unscrambled cube of dimensions 
			//size x size x size with the given SceneManager pointer. The displayed cube can then be 
			//manipulated by the CubeDisplay::rotate function 
			CubeDisplay(const int size, Ogre::SceneManager* scene_mgr);

			//performs the given rotation
			void rotate(const cube::Twist &move);
			

			void set_frames_per_rotation(const int frames) {
				assert(90%frames == 0 && "90 must be divisible by the number of frames to rotate");
				frames_per_rotation = frames;
			}
	
	};
}

#endif
