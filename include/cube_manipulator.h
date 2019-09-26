#ifndef CUBE_MANIPULATOR_H
#define CUBE_MANIPULATOR_H

#include <vector>
#include <array>
#include "Ogre.h"

namespace cube {
	enum class Face;
}

namespace ui {

	//performs nessesary matrix operations to manipulate a cube
	class CubeManipulator {
		private:
			//transposes the given square matrix. Pointers to the elements to be transposed are accepted as arguments
			static void transpose(const std::vector<std::vector<Ogre::SceneNode**>>& matrix);
			
			//reverses the rows in the given matrix. Pointers to the elements to be reversed are accepted as arguments
			static void reverse_rows(const std::vector<std::vector<Ogre::SceneNode**>>& matrix);

			//helper function for CubeManipulator::find_layer
			static std::array<int, 3> layer_coords(const int i, const int j, const cube::Face face, const int layer, const int cube_size);
		public:
			//performs the given rotation on the given matrix. Pointers to the elements that are to be
			//rotated are accepted as arguments
			static void rotate(const std::vector<std::vector<Ogre::SceneNode**>>& matrix, const int degrees);

			//returns a matrix that contains pointers to the elements that make up the specified layer in the given cube
			static std::vector<std::vector<Ogre::SceneNode**>> find_layer(std::vector<std::vector<std::vector<Ogre::SceneNode*>>>& cube, const cube::Face face, const int layer);
			
			//returns a matrix that contains const pointers to the elements that make up the specified layer in the given cube
			static std::vector<std::vector<Ogre::SceneNode* const*>> const_find_layer(const std::vector<std::vector<std::vector<Ogre::SceneNode*>>>& cube, const cube::Face face, const int layer);
	};
}

#endif
